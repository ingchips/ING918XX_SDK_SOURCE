from types import ModuleType
import serial
import time
import icsdw
from icsdw import calc_crc_16, printProgressBar, call_on_batch, call_on_file, do_test, device
from ctypes import create_string_buffer
import struct
import math

SCRIPT_MOD = None

class intf_base(object):
    def __init__(self):
        self.PAGE_SIZE = 256
        self.SECTOR_SIZE = 4 * 1024

        self.BOOT_HELLO    = b'UartBurnStart916\n'
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
        self.CMD_JUMPD   = b'#$jumpd'
        self.CMD_SET_BAUD= b'#$sbaud'
        self.CMD_FLASH_SET  = b'#$fshst'
        self.CMD_LOCK    = b'#$lockk'
        self.CMD_UNLOCK  = b'#$unlck'
        self.CMD_QLOCKSTATE = b'#$state'
        self.CMD_HOLD = b'#$holdd'
        self.CMD_JRAM = b'#$j2ram'

    def is_locked(self, dev):
        exec_cmd = getattr(self, 'exec_cmd')
        rsp = exec_cmd(dev, self.CMD_QLOCKSTATE)
        if rsp[:5] == self.STATUS_LOCKED[:5]:
            return True
        elif rsp[:5] == self.STATUS_UNLOCKED[:5]:
            return False
        else:
            raise Exception("bad response: " + str(rsp))

    def unlock(self, dev):
        exec_cmd = getattr(self, 'exec_cmd')
        return exec_cmd(dev, self.CMD_UNLOCK)[:5] == self.ACK[:5]

    def lock(self, dev):
        exec_cmd = getattr(self, 'exec_cmd')
        return exec_cmd(dev, self.CMD_LOCK)[:5] == self.ACK[:5]

    def prepare(self, x, y, z):
        pass

    def modify_baud(self, x, y):
        pass

class intf_uart(intf_base):
    def __init__(self):
        super(intf_uart, self).__init__()

    def exec_cmd(self, ser: serial.Serial, cmd: bytes):
        ser.write(cmd)
        rsp = ser.read(self.RSP_LEN)
        return rsp

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

    def wait_handshaking(self, ser: serial.Serial, timeout: int):
        import icsdw918
        return icsdw918.wait_handshaking3(ser, timeout, self.BOOT_HELLO)

    def prepare(self, ser: serial.Serial, go, timeout: int):
        if not go:
            if not self.wait_handshaking(ser, timeout):
                print("handshaking timeout")
                return 1
        else:
            ser.reset_input_buffer()

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
                data += b'0'*(size - len(data))
            dev.write(self._config['out_ep'], data, self._config['timeout'])
        response = dev.read(self._config['in_ep'], res_size, self._config['timeout'])
        rsp = struct.unpack_from('<5s',response,0)

        return rsp[0], response[64:]

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
        if self.exec_cmd_4(dev, self.SEND_PAGE if addr < icsdw.RAM_BASE_ADDR else self.SEND_RAM_DATA, 64, next_size, addr, size, data)[0] != self.ACK[:5]:
            return False

        return True

    def roundup_y(self, x, y):
        return round(y*math.ceil(x/y))

    def send_file(self, dev, addr: int, data: bytes):
        offset = 0

        if len(data) == 0:
            return False

        if (addr < icsdw.RAM_BASE_ADDR):
            [self.erase_sector(dev, addr+i*self.SECTOR_SIZE) for i in range(math.ceil(len(data)/self.SECTOR_SIZE))]

        next_size = min(len(data), self.SECTOR_SIZE)

        if self.exec_cmd_4(dev, self.SEND_PAGE if addr < icsdw.RAM_BASE_ADDR else self.SEND_RAM_DATA, 64, self.roundup_y(next_size,256)+64, 0, 0, 0)[0] != self.ACK[:5]:
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

def do_run(mod: ModuleType, d:device, config, go, timeout, counter, user_data):
    global SCRIPT_MOD
    SCRIPT_MOD = mod
    batch_counter = counter
    if batch_counter < 0:
        batch_counter = config.getint('options', 'batch.current')

    intf = intf_uart() if d.dev_type == 0 else intf_usb()

    intf.prepare(d.dev, go, timeout)

    if intf.is_locked(d.dev):
        if config.getboolean('options', 'protection.unlock'):
            intf.unlock(d.dev)
        else:
            print("flash locked")
            return 3

    if config.getboolean('options', 'ResetReservedFlash'):
        intf.erase_sector(d.dev, 0x2000000)

    if config.getint('uart', 'Baud') != icsdw.DEF_BAUD:
        intf.modify_baud(d.dev, config)

    if call_on_batch(SCRIPT_MOD, batch_counter):
        return 10

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
        if not intf.send_file(d.dev, addr, new_data):
            return 4

    if config.getboolean('options', 'set-entry'):
        entry_addr = int(config.get('options', 'entry'), 0)

        if entry_addr >= icsdw.RAM_BASE_ADDR:
            intf.direct_jump(d.dev, entry_addr)
            return do_test(d.dev, config)
        else:
            pass

    if config.getboolean('options', 'protection.enabled'):
        if not intf.lock(d.dev):
            return 6

    if config.getboolean('options', 'launch'):
        intf.launch_app(d.dev)

    return do_test(d.dev, config)