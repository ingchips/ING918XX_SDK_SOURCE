import ctypes
import logging
import argparse
import time
import struct
from datetime import datetime

import logging

import pyocd.utility.color_log
from pyocd.core.helpers import ConnectHelper
from pyocd.core.soc_target import SoCTarget
from pyocd.probe import aggregator
from pyocd.utility.kbhit import KBHit

from ctypes import Structure, c_char, c_int32, c_uint32, sizeof

LOG = logging.getLogger(__name__)

RTT_MAGIC = 'SEGGER RTT'

class SEGGER_RTT_BUFFER_UP(ctypes.Structure):
    _fields_ = [
        ('sName',        ctypes.c_uint32),
        ('pBuffer',      ctypes.c_uint32),
        ('SizeOfBuffer', ctypes.c_uint32),
        ('WrOff',        ctypes.c_uint32),
        ('RdOff',        ctypes.c_uint32),
        ('Flags',        ctypes.c_uint32),
    ]

class SEGGER_RTT_BUFFER_DOWN(ctypes.Structure):
    _fields_ = [
        ('sName',        ctypes.c_uint32),
        ('pBuffer',      ctypes.c_uint32),
        ('SizeOfBuffer', ctypes.c_uint32),
        ('WrOff',        ctypes.c_uint32),
        ('RdOff',        ctypes.c_uint32),
        ('Flags',        ctypes.c_uint32),
    ]

class SEGGER_RTT_CB(ctypes.Structure):      # Control Block
    _fields_ = [
        ('acID',              ctypes.c_char * 16),
        ('MaxNumUpBuffers',   ctypes.c_uint32),
        ('MaxNumDownBuffers', ctypes.c_uint32),
    ]

def get_default_probe_id():
    daplinks = aggregator.DebugProbeAggregator.get_all_connected_probes()

    if len(daplinks) == 0:
        LOG.error("NO DAP-Link")
        exit(-1)

    if len(daplinks) == 1:
        return daplinks[0].unique_id

    LOG.info("Multiple DAP-Link found. Use -ID .. to specify one:\n")
    for prob in daplinks:
        LOG.info(prob.unique_id)

    exit(-2)

def find_rtt_ctx(FLAGS, dap):
    if FLAGS.RTTAddress > 0:
        buff = dap.read_memory_block8(FLAGS.RTTAddress, 32)
        buff = ''.join([chr(x) for x in buff])
        if buff.find(RTT_MAGIC) == 0:
            return FLAGS.RTTAddress
        else:
            raise Exception(f"SEGGER RTT not found at 0x{FLAGS.RTTAddress:0>8x}")

    search = [int(x, 0) for x in FLAGS.RTTSearchRanges.split()]
    if len(search) != 2:
        raise Exception(f"Search range '{FLAGS.RTTSearchRanges}' is invalid")

    start = search[0]
    remain = search[1]
    while remain > 0:
        buff = dap.read_memory_block8(start, 512 + 16)
        buff = ''.join([chr(x) for x in buff])
        index = buff.find(RTT_MAGIC)
        if index >= 0:
            return start + index
        start += 512
        remain -= 512

    raise Exception(f"SEGGER RTT not found in specified range")

def make_file_header(pre_size: int):
    now = datetime.now()
    milliseconds_since_midnight = int((now - now.replace(hour=0, minute=0, second=0, microsecond=0)).total_seconds() * 1000)
    padded1 = bytearray(b'\xff' * 8)
    padded1[1::2] = struct.pack('<I', milliseconds_since_midnight)
    padded2 = bytearray(b'\xff' * 8)
    padded2[1::2] = struct.pack('<I', pre_size)
    return b'\xff\x54\xff\x52\xff\x41\xFF\x01' + bytes(padded1) + bytes(padded2)

def run(FLAGS):
    pyocd.utility.color_log.build_color_logger(color_setting='auto')

    if FLAGS.ID == '':
        FLAGS.ID = get_default_probe_id()
    LOG.setLevel(logging.DEBUG)
    kb = None
    try:
        session = ConnectHelper.session_with_chosen_probe(
            project_dir=None,
            config_file=None,
            user_script=None,
            no_config=True,
            pack=None,
            unique_id=FLAGS.ID,
            target_override=FLAGS.Device,
            frequency=FLAGS.Speed * 1000,
            blocking=False,
            connect_mode=FLAGS.ConnectMode,
            options={})

        if session is None:
            LOG.error("No target device available")
            return 1

        with session, open(FLAGS.LogFile, 'wb') as log_file:

            target: SoCTarget = session.board.target

            rtt_cb_addr = find_rtt_ctx(FLAGS, target)

            data = target.read_memory_block8(rtt_cb_addr, sizeof(SEGGER_RTT_CB))
            rtt_cb = SEGGER_RTT_CB.from_buffer(bytearray(data))
            up_addr = rtt_cb_addr + sizeof(SEGGER_RTT_CB) + sizeof(SEGGER_RTT_BUFFER_UP) * FLAGS.RTTChannel

            LOG.info(f"_SEGGER_RTT @ {rtt_cb_addr:#08x} with {rtt_cb.MaxNumUpBuffers} aUp and {rtt_cb.MaxNumDownBuffers} aDown")

            target.resume()

            # set up terminal input
            kb = KBHit()

            LOG.info("start logging...")
            total_size = 0
            block_size = 0
            last_time = time.time()
            error_count = 0
            MAX_ERRORS = 10

            def is_rtt_empty():
                data = target.read_memory_block8(up_addr, sizeof(SEGGER_RTT_BUFFER_UP))
                up = SEGGER_RTT_BUFFER_UP.from_buffer(bytearray(data))
                return up.WrOff == up.RdOff

            def read_up_channel() -> bytes:
                nonlocal error_count, MAX_ERRORS, up_addr

                data = target.read_memory_block8(up_addr, sizeof(SEGGER_RTT_BUFFER_UP))
                up = SEGGER_RTT_BUFFER_UP.from_buffer(bytearray(data))

                if up.WrOff == up.RdOff:
                    return bytes([])

                # Validate buffer pointers
                if up.WrOff >= up.SizeOfBuffer or up.RdOff >= up.SizeOfBuffer:
                    error_count += 1
                    return bytes([])

                if up.WrOff > up.RdOff:
                    # Normal case - read contiguous block
                    read_size = up.WrOff - up.RdOff
                    data = target.read_memory_block8(up.pBuffer + up.RdOff, read_size)

                elif up.WrOff < up.RdOff:
                    # Wrap-around case
                    first_part = up.SizeOfBuffer - up.RdOff
                    data = target.read_memory_block8(up.pBuffer + up.RdOff, first_part)

                    second_part = up.WrOff
                    if second_part > 0:
                        data += target.read_memory_block8(up.pBuffer, second_part)

                target.write_memory(up_addr + SEGGER_RTT_BUFFER_UP.RdOff.offset, up.WrOff)
                return bytes(data)

            data = read_up_channel()
            while is_rtt_empty():
                time.sleep(0.01)

            log_file.write(make_file_header(len(data)))
            log_file.write(data)

            while True:
                data = read_up_channel()
                if len(data) == 0:
                    if error_count > MAX_ERRORS:
                        LOG.error("too many errors, abort...")
                        break
                    if kb.kbhit():
                        LOG.info("Abort...")
                        break
                    time.sleep(0.001)  # Short delay to prevent busy waiting
                    continue

                log_file.write(bytes(data))
                error_count = 0  # Reset error counter on successful read

                s = len(data)
                block_size += s
                total_size += s
                diff = time.time() - last_time
                if diff > 1.0:
                    print(f"Transfer rate: {block_size / 1000:.1f} KByte/s; Bytes written: {total_size / 1000:.0f} KByte", end="\r")
                    block_size = 0
                    last_time = time.time()

    except KeyboardInterrupt:
        pass

    finally:
        if session:
            session.close()
        if kb:
            kb.set_normal_term()

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '-If',
        type=str,
        default='SWD',
        help='interface')
    parser.add_argument(
        '-Device',
        type=str,
        default='cortex_m',
        help='device')
    parser.add_argument(
        '-Speed',
        type=int,
        default=8000,
        help='speed')
    parser.add_argument(
        '-RTTChannel',
        type=int,
        default=0,
        help='RTT channel')
    parser.add_argument(
        '-RTTSearchRanges',
        type=str,
        default="",
        help='RTT search ranges')
    parser.add_argument(
        '-RTTAddress',
        type=lambda x: int(x, 0),
        default=-1,
        help='RTT search ranges')
    parser.add_argument(
        '-ID',
        type=str,
        default="",
        help='Probe unique ID')
    parser.add_argument(
        '-ConnectMode',
        type=str,
        default="attach",
        help='Select connect mode from one of (halt, pre-reset, under-reset, attach)')
    parser.add_argument(
        'LogFile',
        type=str,
        help='Log file name')
    FLAGS, unparsed = parser.parse_known_args()
    run(FLAGS)
