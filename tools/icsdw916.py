from types import ModuleType
import serial
import time
import icsdw
from icsdw import calc_crc_16, printProgressBar, call_on_batch, call_on_file, do_test, device
from ctypes import create_string_buffer
import struct
import math

SCRIPT_MOD = None
BOOT_HELLO = b'UartBurnStart916\n'
AHB_QSPI_MEM_BASE = 0x04000000

class intf_base(object):
    def __init__(self):
        global BOOT_HELLO

        self.PAGE_SIZE = 256
        self.SECTOR_SIZE = 4 * 1024

        self.BOOT_HELLO    = BOOT_HELLO
        self.ACK           = b'#$ack\n'
        self.NACK          = b'#$nak\n'
        self.STATUS_LOCKED   = b'#$lck\n'
        self.STATUS_UNLOCKED = b'#$ulk\n'

        self.RSP_LEN = len(self.ACK)

        self.SEND_PAGE   = b'#$u2fsh'
        self.READ_PAGE   = b'#$fsh2u'
        self.SEND_RAM_DATA=b'#$u2ram'
        self.ERASE_SECTOR  = b'#$stera'
        self.CMD_LAUNCH  = b'#$j2fsh'
        self.CMD_SET_BAUD= b'#$sbaud'
        self.CMD_FLASH_SET  = b'#$fshst'
        self.CMD_LOCK    = b'#$lockk'
        self.CMD_UNLOCK  = b'#$unlck'
        self.CMD_QLOCKSTATE = b'#$state'
        self.CMD_HOLD = b'#$holdd'
        self.CMD_JRAM = b'#$j2ram'

        self.erase_sectors = True

    def exec_cmd(self, dev, cmd: bytes): ...

    def is_locked(self, dev):
        rsp = self.exec_cmd(dev, self.CMD_QLOCKSTATE)
        if rsp[:5] == self.STATUS_LOCKED[:5]:
            return True
        elif rsp[:5] == self.STATUS_UNLOCKED[:5]:
            return False
        else:
            raise Exception("bad response: " + str(rsp))

    def unlock(self, dev):
        return self.exec_cmd(dev, self.CMD_UNLOCK)[:5] == self.ACK[:5]

    def lock(self, dev):
        return self.exec_cmd(dev, self.CMD_LOCK)[:5] == self.ACK[:5]

    def prepare(self, x, y, z):
        return True

    def modify_baud(self, x, y):
        pass

    def set_flash(self, dev, code): ...

    def set_flash_qspi(self, dev):
        return self.set_flash(dev, 0x0200)

    def set_flash_write_protection(self, dev):
        return self.set_flash(dev, 0x027c)

class intf_uart(intf_base):
    def __init__(self):
        super(intf_uart, self).__init__()

    def exec_cmd(self, ser: serial.Serial, cmd: bytes):
        ser.write(cmd)
        rsp = ser.read(self.RSP_LEN)
        return rsp

    def set_flash(self, ser: serial.Serial, code):
        return self.exec_cmd(ser, self.CMD_FLASH_SET + code.to_bytes(2, 'little')) == self.ACK

    def launch_app(self, ser: serial.Serial):
        ser.write(self.CMD_LAUNCH)

    def direct_jump(self, ser: serial.Serial, addr: int):
        return self.exec_cmd(ser, self.CMD_JRAM + addr.to_bytes(4, 'little')) == self.ACK

    def erase_sector(self, ser: serial.Serial, addr: int):
        return self.exec_cmd(ser, self.ERASE_SECTOR + addr.to_bytes(4, 'little')) == self.ACK

    def set_baud(self, ser: serial.Serial, baud: int):
        rsp = self.exec_cmd(ser, self.CMD_SET_BAUD + baud.to_bytes(4, 'little'))
        return rsp == self.ACK

    def send_page(self, ser: serial.Serial, addr: int, data: bytes):
        CMD = self.SEND_PAGE if addr < icsdw.RAM_BASE_ADDR else self.SEND_RAM_DATA
        rsp = self.exec_cmd(ser, CMD + addr.to_bytes(4, 'little') + (len(data) - 1).to_bytes(1, 'little'))
        if rsp != self.ACK:
            return False
        ser.write(data)
        crc = calc_crc_16(data)
        ser.write(crc.to_bytes(2, 'little'))
        rsp = ser.read(len(self.ACK))
        return rsp == self.ACK

    def send_sector(self, ser: serial.Serial, addr: int, data: bytes):
        if (addr < icsdw.RAM_BASE_ADDR) and (not self.erase_sector(ser, addr)):
            return False

        offset = 0
        while len(data) > offset:
            seg = min(len(data) - offset, self.PAGE_SIZE)

            if not self.send_page(ser, addr + offset, data[offset:offset + seg]):
                return False

            offset += seg

        return True

    def send_file(self, ser: serial.Serial, addr: int, data: bytes):
        offset = 0
        while len(data) > offset:
            seg = min(len(data) - offset, self.SECTOR_SIZE)

            if not self.send_sector(ser, addr + offset, data[offset:offset + seg]):
                return False

            offset += seg

            printProgressBar(offset, len(data))

        return True

    def wait_handshaking(self, ser: serial.Serial, timeout: float):
        import icsdw918
        return icsdw918.wait_handshaking3(ser, timeout, self.BOOT_HELLO)

    def prepare(self, ser: serial.Serial, go, timeout: int):
        if not go:
            if not self.wait_handshaking(ser, timeout):
                print("handshaking timeout")
                return False
        else:
            ser.reset_input_buffer()

        return True

    def modify_baud(self, ser: serial.Serial, config):
        print('baud -> {}'.format(config.getint('uart', 'Baud')))
        if not self.set_baud(ser, config.getint('uart', 'Baud')):
            return 3
        time.sleep(0.1)
        ser.baudrate = config.getint('uart', 'Baud')

    def read_page(self, ser: serial.Serial, addr):
        ser.write(self.READ_PAGE + addr.to_bytes(4, 'little'))
        return ser.read(self.PAGE_SIZE)

class intf_usb(intf_base):
    def __init__(self):
        super(intf_usb, self).__init__()
        self._config = dict(out_ep=0x01, in_ep=0x82, mps=64, timeout=100)

    def exec_cmd(self, dev, cmd: bytes):
        buffer = create_string_buffer(64)
        struct.pack_into('<7s', buffer, 0, cmd)
        struct.pack_into('<I', buffer, 16, 64)
        struct.pack_into('<I', buffer, 20, 64)

        assert dev.write(self._config['out_ep'], buffer.raw, self._config['timeout']) == self._config['mps']
        response = dev.read(self._config['in_ep'], self._config['mps'], self._config['timeout'])
        rsp = struct.unpack_from('<5s',response,0)

        return rsp[0]

    def exec_cmd_2(self, dev, cmd: bytes, addr, size):
        buffer = create_string_buffer(64)
        struct.pack_into('<7s', buffer, 0, cmd)
        struct.pack_into('<I', buffer, 16, 64)
        struct.pack_into('<I', buffer, 20, 64)
        struct.pack_into('<I', buffer, 24, addr)
        struct.pack_into('<I', buffer, 28, size)

        assert dev.write(self._config['out_ep'], buffer.raw, self._config['timeout']) == self._config['mps']
        response = dev.read(self._config['in_ep'], self._config['mps'], self._config['timeout'])
        rsp = struct.unpack_from('<5s',response,0)

        return rsp[0]

    def exec_cmd_4(self, dev, cmd: bytes, res_size, next_c_size, addr, size, data):
        buffer = create_string_buffer(64)
        struct.pack_into('<7s', buffer, 0, cmd)
        struct.pack_into('<I', buffer, 16, res_size)
        struct.pack_into('<I', buffer, 20, next_c_size)
        struct.pack_into('<I', buffer, 24, addr)
        struct.pack_into('<I', buffer, 28, size)

        assert dev.write(self._config['out_ep'], buffer.raw, self._config['timeout']) == self._config['mps']
        if data != 0:
            if len(data) < size:
                data += b'\x00'*(size - len(data))
            dev.write(self._config['out_ep'], data, self._config['timeout'])
        response = dev.read(self._config['in_ep'], res_size, self._config['timeout'])
        rsp = struct.unpack_from('<5s',response,0)

        return rsp[0], response[64:]

    def set_flash(self, dev, code):
        # This prelude command is to tell USB boot-loader total size of the next command
        self.exec_cmd_4(dev, b'#$hello', 64, 64 + 2, 0, 0, 0)
        r0, r = self.exec_cmd_4(dev, self.CMD_FLASH_SET, 64, 64, 0, 2, code.to_bytes(4, 'little'))
        return r0 == self.ACK[:5]

    def launch_app(self, dev):
        buffer = create_string_buffer(64)
        struct.pack_into('<7s', buffer, 0, self.CMD_LAUNCH)
        struct.pack_into('<I', buffer, 16, 64)
        struct.pack_into('<I', buffer, 20, 64)

        assert dev.write(self._config['out_ep'], buffer.raw, self._config['timeout']) == self._config['mps']

    def direct_jump(self, dev, addr: int):
        return self.exec_cmd_2(dev, self.CMD_JRAM, addr, 0) == self.ACK[:5]

    def erase_sector(self, dev, addr: int):
        return self.exec_cmd_2(dev, self.ERASE_SECTOR, addr, 0) == self.ACK[:5]

    def send_sector(self, dev, addr: int, data: bytes, size, next_size):
        if self.exec_cmd_4(dev, self.SEND_PAGE if addr < icsdw.RAM_BASE_ADDR else self.SEND_RAM_DATA,
                           64, next_size, addr, size, data)[0] != self.ACK[:5]:
            return False

        return True

    def roundup_y(self, x, y):
        return round(y*math.ceil(x/y))

    def send_file(self, dev, addr: int, data: bytes):
        offset = 0

        if len(data) == 0:
            return False

        if self.erase_sectors and (addr < icsdw.RAM_BASE_ADDR):
            addresses = range(math.ceil(len(data)/self.SECTOR_SIZE))
            for i in addresses:
                self.erase_sector(dev, addr + i * self.SECTOR_SIZE)
                printProgressBar(i, len(addresses))

        next_size = min(len(data), self.SECTOR_SIZE)

        if self.exec_cmd_4(dev, self.SEND_PAGE if addr < icsdw.RAM_BASE_ADDR else self.SEND_RAM_DATA,
                           64, self.roundup_y(next_size,256) + 64, 0, 0, 0)[0] != self.ACK[:5]:
            return False

        while len(data) > offset:
            seg = min(len(data) - offset, self.SECTOR_SIZE)
            next_size = self.roundup_y(min(len(data) - offset - seg, self.SECTOR_SIZE),256)+64 if (len(data) - offset - seg) != 0 else 64

            if not self.send_sector(dev, addr + offset, data[offset:offset + seg], self.roundup_y(seg,256), next_size):
                return False

            offset += seg

            printProgressBar(offset, len(data))

        return True

    def read_page(self, dev, addr):
        return self.exec_cmd_4(dev, self.READ_PAGE, self.PAGE_SIZE+64, 64, addr, self.PAGE_SIZE, 0)[1]

def download_external_flash_bootloader(intf, d: device, flashtype: int) -> bool:
    import base64
    loaders = ["""
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
AMGBQAHAgEEAwYFAAMGBQAHAgEEAwYFAAcCAQQHAgEEAwYFAAMGBQAHAgEEBwIBBAMGBQAHAgEEA
wYFAAMGBQAHAgEEBwIBBAMGBQADBgUABwIBBAMGBQAHAgEEBwIBBAMGBQADBgUABwIBBAcCAQQDB
gUABwIBBAMGBQADBgUABwIBBAMGBQAHAgEEBwIBBAMGBQAHAgEEAwYFAAMGBQAHAgEEBwIBBAMGB
QADBgUABwIBBAMGBQAHAgEEBwIBBAMGBQADAwQHDAwLCxgYHxwXFxATMDA3ND8/ODgrKywvJCQjI
2BgZ2Rvb2hoe3t8f3R0c3BTU1RXXFxbW0hIT0xHR0BDwMDHxM/PyMjb29zf1NTT0PPz9Pf8/Pv76
Ojv7Ofn4OCjo6SnrKyrq7i4v7y3t7CzkJCXlJ+fmJiLi4yPhISDgoGBhoWOjomJmpqdnpWVkpGys
rW2vb26uqmprq2mpqGh4uLl5u3t6ur5+f799vbx8tHR1tXe3tnZysrNzsXFwsFCQkVGTU1KSllZX
l1WVlFScXF2dX5+eXlqam1uZWViYiEhJiUuLikpOjo9PjU1MjESEhUWHR0aGgkJDg0GBgEAAAAAA
BwAHAAEAAQAAARAAAAAAAAAAAAAHAAIAAQABAAABEAAAAAAAAAAAAAcAAQADAAEAAAEQAAAAAAAA
AAAABwABAAAAAQAAARAAAAAAAAAAAAAHAAEAAgABAAABEAAAAAAAuLcAIAAAACBYAAAAUpkAIBC4
ACBYAAAgEBAAAGKZACAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAQm9vdGxvYWRlcgBCb290
bG9hZGVyIEludGVyZmFjZQBJbmdjaGlwcyBVU0IgRGV2aWNlAAAAAAAAAAAA"""]
    if (flashtype < 0) or (flashtype >= len(loaders)):
        raise Exception(f'unsupported external flash type: {flashtype}')
    bin = base64.b64decode(''.join(loaders[flashtype].splitlines()))

    entry_addr = 0x20008000
    print('downloading external flash bootloader @ {:#x} ...'.format(entry_addr))
    if not intf.send_file(d.dev, entry_addr, bin):
        return False

    intf.direct_jump(d.dev, entry_addr)
    if isinstance(intf, intf_uart):
        time.sleep(0.2)
        d.dev.read_all()
    if isinstance(intf, intf_usb):
        d.close()
        time.sleep(1.0)
        d.reopen()
    return True

def prepare_loader(intf: intf_base, d: device, config) -> bool:
    use_ram = False
    use_ext = config.getboolean('options', 'AlwaysUseExtBootloader', fallback=False)
    for i in range(6):
        bcfg = dict(config.items('bin-' + str(i)))
        if bcfg['checked'] != '1':
            continue

        addr = int(bcfg['address'])
        if addr >= icsdw.RAM_BASE_ADDR:
            use_ram = True
        elif addr >= AHB_QSPI_MEM_BASE:
            use_ext = True

    if use_ram and use_ext:
        raise Exception(f'Not supported: download to RAM and external flash in a single project')

    if not use_ext: return True

    if isinstance(intf, intf_usb):
        intf.erase_sectors = False

    return download_external_flash_bootloader(intf, d,  config.getint('options', 'ExtFlashType', fallback=0))

def do_run(mod: ModuleType, d: device, config, go, timeout, counter, user_data):
    global SCRIPT_MOD
    SCRIPT_MOD = mod
    batch_counter = counter
    if batch_counter < 0:
        batch_counter = config.getint('options', 'batch.current')

    intf = intf_uart() if d.dev_type == 0 else intf_usb()

    if not intf.prepare(d.dev, go, timeout):
        return 1

    if intf.is_locked(d.dev):
        if config.getboolean('options', 'protection.unlock'):
            intf.unlock(d.dev)
        else:
            print("flash locked")
            return 3

    if not intf.set_flash_qspi(d.dev):
        return 20

    if config.getint('uart', 'Baud') != icsdw.DEF_BAUD:
        intf.modify_baud(d.dev, config)

    if config.getboolean('options', 'ResetReservedFlash', fallback=False):
        intf.erase_sector(d.dev, 0x2000000)

    if not prepare_loader(intf, d, config):
        return 21

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
        if not intf.send_file(d.dev, addr, new_data):
            return 4
        meas.show_throughput(len(new_data))

    if config.getboolean('options', 'set-entry'):
        entry_addr = int(config.get('options', 'entry'), 0)

        if entry_addr >= icsdw.RAM_BASE_ADDR:
            intf.direct_jump(d.dev, entry_addr)
            return do_test(d.dev, config)
        else:
            pass

    if config.getboolean('options', 'writeprotection.enabled', fallback=False):
        intf.set_flash_write_protection(d.dev)

    if config.getboolean('options', 'protection.enabled', fallback=False):
        if not intf.lock(d.dev):
            return 6

    if config.getboolean('options', 'launch'):
        intf.launch_app(d.dev)

    return do_test(d.dev, config)