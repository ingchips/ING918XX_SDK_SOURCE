from types import ModuleType
import serial
import time
import icsdw
from icsdw import calc_crc_16, printProgressBar, call_on_batch, call_on_file, do_test

PAGE_SIZE = 8 * 1024

BOOT_HELLO    = b'UartBurnStart\n'
ACK           = b'#$ack\n'
NACK          = b'#$nak\n'
STATUS_LOCKED   = b'#$lck\n'
STATUS_UNLOCKED = b'#$ulk\n'

RSP_LEN = len(ACK)

SEND_PAGE   = b'#$start'
READ_PAGE   = b'#$readd'
SET_ENTRY   = b'#$setja'
CMD_LAUNCH  = b'#$jumpp'
CMD_JUMPD   = b'#$jumpd'
CMD_SET_BAUD= b'#$sbaud'
CMD_LOCK    = b'#$lockk'
CMD_UNLOCK  = b'#$unlck'
CMD_QLOCKSTATE = b'#$state'

SCRIPT_MOD = None

def exec_cmd(ser: serial.Serial, cmd: bytes):
    ser.write(cmd)
    rsp = ser.read(RSP_LEN)
    return rsp

def is_locked(ser: serial.Serial):
    rsp = exec_cmd(ser, CMD_QLOCKSTATE)
    if rsp == STATUS_LOCKED:
        return True
    elif rsp == STATUS_UNLOCKED:
        return False
    else:
        raise Exception("bad response: " + str(rsp))

def set_entry(ser: serial.Serial, entry: int):
    return exec_cmd(ser, SET_ENTRY + entry.to_bytes(4, 'little')) == ACK

def launch_app(ser: serial.Serial):
    ser.write(CMD_LAUNCH)

def direct_jump(ser: serial.Serial, addr: int):
    return exec_cmd(ser, CMD_JUMPD + addr.to_bytes(4, 'little')) == ACK

def unlock(ser: serial.Serial):
    return exec_cmd(ser, CMD_UNLOCK) == ACK

def lock(ser: serial.Serial):
    return exec_cmd(ser, CMD_LOCK) == ACK

def set_baud(ser: serial.Serial, baud: int):
    rsp = exec_cmd(ser, CMD_SET_BAUD + baud.to_bytes(4, 'little'))
    return rsp == ACK

def send_page(ser: serial.Serial, addr: int, data: bytes):
    CMD = SEND_PAGE if addr < icsdw.RAM_BASE_ADDR else b'#$stram'
    rsp = exec_cmd(ser, CMD + addr.to_bytes(4, 'little') + len(data).to_bytes(2, 'little'))
    if rsp != ACK:
        return False
    ser.write(data)
    crc = calc_crc_16(data)
    ser.write(crc.to_bytes(2, 'little'))
    rsp = ser.read(len(ACK))
    return rsp == ACK

def send_file(ser: serial.Serial, addr: int, data: bytes):
    offset = 0
    while len(data) > offset:
        seg = len(data) - offset
        seg = PAGE_SIZE if seg > PAGE_SIZE else seg

        if not send_page(ser, addr + offset, data[offset:offset + seg]):
            return False

        offset += seg

        printProgressBar(offset, len(data))

    return True

def wait_handshaking3(ser: serial.Serial, timeout: float, hello: str):
    print("wait for handshaking...", end = '\r')
    ser.rts = True
    ser.dtr = True
    time.sleep(0.1)
    ser.dtr = False
    ser.rts = True
    start = time.time()
    acc = bytes()
    while True:
        acc = acc + ser.read(1)
        if len(acc) >= len(hello):
            if acc[-len(hello):] == hello:
                print("                               ", end = '\r')
                return True
        if time.time() - start > timeout:
            print()
            return False

def wait_handshaking(ser: serial.Serial, timeout: float):
    return wait_handshaking3(ser, timeout, BOOT_HELLO)

def do_run(mod: ModuleType, ser, config, go, timeout, counter, user_data):
    global SCRIPT_MOD
    SCRIPT_MOD = mod
    batch_counter = counter
    if batch_counter < 0:
        batch_counter = config.getint('options', 'batch.current')

    if not go:
        if not wait_handshaking(ser, timeout):
            print("handshaking timeout")
            return 1
    else:
        ser.reset_input_buffer()

    if is_locked(ser):
        if config.getboolean('options', 'protection.unlock'):
            unlock(ser)
        else:
            print("flash locked")
            return 3

    if config.getint('uart', 'Baud') != icsdw.DEF_BAUD:
        print('baud -> {}'.format(config.getint('uart', 'Baud')))
        if not set_baud(ser, config.getint('uart', 'Baud')):
            return 3
        time.sleep(0.1)
        ser.baudrate = config.getint('uart', 'Baud')

    if call_on_batch(SCRIPT_MOD, batch_counter):
        return 10

    meas = icsdw.TimeMeasurement()

    for i in range(6):
        bcfg = dict(config.items('bin-' + str(i)))
        if bcfg['checked'] != '1':
            continue

        addr = int(bcfg['address'])

        print('downloading {} @ {:#x} ...'.format(bcfg['filename'], addr))
        data = open(bcfg['filename'], "rb").read()
        abort, new_data = call_on_file(SCRIPT_MOD, batch_counter, i + 1, data, user_data)
        if abort:
            return 10

        meas.start()
        if not send_file(ser, addr, new_data):
            return 4
        meas.show_throughput(len(new_data))

    if config.getboolean('options', 'set-entry'):
        entry_addr = int(config.get('options', 'entry'), 0)

        if entry_addr >= icsdw.RAM_BASE_ADDR:
            direct_jump(ser, entry_addr)
            return do_test(ser, config)
        else:
            if not set_entry(ser, entry_addr):
                return 5

    if config.getboolean('options', 'protection.enabled'):
        if not lock(ser):
            return 6

    if config.getboolean('options', 'launch'):
        launch_app(ser)

    return do_test(ser, config)