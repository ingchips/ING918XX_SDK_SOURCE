from types import ModuleType
import serial
import time
import icsdw
from icsdw import calc_crc_16, printProgressBar, call_on_batch, call_on_file, do_test, device
from ctypes import create_string_buffer
import struct
import math

SCRIPT_MOD = None
AHB_QSPI_MEM_BASE   = 0x04000000

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
        self.CMD_SET_BAUD= b'#$sbaud'
        self.CMD_FLASH_SET  = b'#$fshst'
        self.CMD_LOCK    = b'#$lockk'
        self.CMD_UNLOCK  = b'#$unlck'
        self.CMD_QLOCKSTATE = b'#$state'
        self.CMD_HOLD = b'#$holdd'
        self.CMD_JRAM = b'#$j2ram'

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

        if (addr < icsdw.RAM_BASE_ADDR):
            [self.erase_sector(dev, addr + i * self.SECTOR_SIZE) for i in range(math.ceil(len(data)/self.SECTOR_SIZE))]

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
aBAAINkkACDdJAAg3yQAIOEkACDjJAAg5SQAIAAAAAAAAAAAAAAAAAAAAADnJAAg6SQAIAAAAADr
JAAg7SQAIO8kACDvJAAg7yQAIO8kACDvJAAg7yQAIO8kACDvJAAg7yQAIO8kACDvJAAg7yQAIO8k
ACDvJAAg7yQAIO8kACDvJAAg7yQAIO8kACDvJAAg7yQAIO8kACDvJAAg7yQAIO8kACDvJAAg7yQA
IO8kACDvJAAg7yQAIO8kACDNKQAg3/gQ0ADwWPoASABHcVsAIK/zAIBoEAAg1EgBaE/qUQIS8AEP
Q9DSSAFob/R/QgHqAgFP9ABiQeoCAQFgzE85aG/0wCIB6gIBOWDKTzloykoB6gIBOWDJT0/w/wE5
YMhIAWgR8AEP+tHGSxxoT/AABR1gT/AAcAFoHGBP6hEiT+oRM0/wAQQC6gQCA+oEA0/qgnJP6kNz
vEgBaG/wwEQB6gQBQeoCAUHqAwEBYBXgsE85aG/0wCIB6gIBOWCtTzlorUoB6gIBOWCsT0/w/wE5
YKtIAWgR8AEP+tGrSAFoEfABDwDwBYFP6lECEvABDzTQT+pREk/w/wMD6gIDT+rDE6JKFWhv9P9G
BeoGBUXqAwUVYJ9LHWhP9IAXReoHBR1gT+qRAhLwAQ8H0JhKE2hP6pN0FPABD/jQD+BP6tECT/AD
AwPqAgNP8HwHB/sD9wfyqicAJnYcvkL800/qUTIS8AEPGtBP6pEyT/APAwPqAgNP8IBCFGhv8A8F
BOoFBETqAwRP8IBTROoDBBRggUoTaE/0gERE6gMEFGBP6pFCEvABDy7QT+rRQk/wDwMD6gIDT/CA
QhRob/RwJQTqBQRP6gNFROoFBBRgckoTaE/wAFRE6gMEFGBP9IAkROoDBBRgT+oRYk/wBwMD6gID
YEoUaG/wcAUE6gUET+oDFUTqBQQUYE/q0VIS8AEPCdBYShNoT/ABBEPqBAMTYFZLXkoaYE/q0WIS
8AEPQNBP8IBST/A8A0PqAgNZShRoROoDBBRgV0oUYFdKWEtYTBRgHGBYShNoT+pTFBTwAQ/40VVK
VktP8AB0FGAcYFRKVUtP8MBkFGAcYE/w/zRSSlNLFGAcYFJKU0sUYBxgUkpTSxRgHGBSSlNLFGAc
YFJKU0sUYBxgT/ACBFFKUksUYBxgT+oRchLwAQ8c0E9KE2hP8AIEQ+oEAxNgTEoTaE/0ADRD6gQD
E2BKShNob/TwNAPqBAMTYEdKRfalIxNgRkpA8olzE2AlSAFoT+pRchLwAQ8E0EJIT/AAAgJgA+A/
SE/wAwICYE/qkXIS8AEPBNAaSE/wAAICYA/gF0tP8AAFHWBP8ABwBGhP8P8CBOoCBE/0gHJE6gIE
HGBP6tFyEvABDwfRT/AAUC5JAmiC8wiICmgQRyxILEkCaILzCIgKaBBHAAEQQBQAFUAEABVA/wD+
/wAAFUAMABVAwAEAQBQgEEBAAABAqAEQQAQAAECgAAABQAAUQEAQFEBYABRAWBAUQAQAAIBIEBRA
BAAUQAQQFEAIABRACBAUQAwAFEAMEBRAEAAUQBAQFEAUABRAFBAUQBgAFEAYEBRAHAAUQBwQFEAA
ABRAABAUQAwAAEAUAABAQAAQQBgQAEAQEABAfAAAQAQAACAAIAACBCAAAgVIAEf+5/7n/uf+5/7n
/uf+5/7n/uf+58EgACBA6gEDmwcD0AngCMkSHwjABCr60gPgEfgBOwD4ATtSHvnScEfSsgHgAPgB
K0ke+9JwRwAi9ucQtRNGCkYERhlG//fw/yBGEL1CHBD4ARsAKfvRgBpwRxC1ACIA4FIcg1yMXKNC
AdEAK/jR2LLhskAaEL0DRhH4ASsA+AErACr50RhGcEcGTAdNBuDgaEDwAQOU6AcAmEcQNKxC9tP/
95r9QF8AIGBfACDwtQVGDkYURjFGKEYB8IT7B0anQgHRASDwvQAg/OcAADi1bUYAJAC/A+AA8Jf9
KFVkHAQs+dMFoADwhPwFSgAhBUgAmwDwpP44vQAAIyRhY2sAAAAANm4BABABQAVIgGkBKAXRACAD
SchhASCIYXBHACD85wAgAEAQtQJGC0YAIQfgUFxcXKBCAdAAIBC9SBzBsgcp9dsBIPjnAAAQtShI
AGgg9ABwAPUAcCVJCGAAvyNIAGjA84AgACj50CBIBGzgsgDwWf3E8wcgAPBV/cTzB0AA8FH9IA4A
8E79GEhEbOCyAPBJ/cTzByAA8EX9xPMHQADwQf0gDgDwPv0QSIRs4LIA8Dn9xPMHIADwNf3E8wdA
APAx/SAOAPAu/QhIxGzgsgDwKf3E8wcgAPAl/cTzB0AA8CH9IA4A8B79EL0AAABwAUAOtRhIAGgg
9ABwAPUAcBVJCGAAvxNIAGjA84AgACj50GpGAakCqAHw1/wOSABsAJkIYACYAB0AkApIQGwAmQhg
AJgAHQCQB0iAbACZCGAAmAAdAJADSMBsAJkIYAKgAfDX/A69AHABQCMkYWNrAAAAcLUERg1GFkYA
IAHwx/qk8YBgMkYpRgHw+foBIAHwvvpwvQAAALUCRgDwyvii8QByAPAA+VIgQOoCIARJCGAA8L/4
APDP+ADwu/gAvQAAFUAAtQDwtfgA8O34YCAESQhgAPCu+ADwvvgA8Kr4AL0AAAAAFUAQtRlIAGjA
8wAjT/CAQND4wAEg9IBwFEwgYADwl/gTSEBoE0wgQED0AHAQTGBgBSAgYADwi/ggRgBpwbIKRjUg
IGAA8IP4IEYAacGyQuoBIgDwfPigA9D4wAFj8wggAkwgYBBGEL0AAMABAEAAABVA/wD+/zC1A0YM
RhVGAPBm+KPxAHMAIgPgoFwJSYhgUhyqQvnTAPCU+AIgAOsDIARJCGAA8FP4APBj+ADwT/gwvQAA
FUAAtQJGAPBI+KLxAHIA8H74ICBA6gIgBEkIYADwPfgA8E34APA5+AC9AAAVQAC1AkYWSABowPMA
I0/wgEDQ+MABIPSAcBFJCGAA8Cb4EEhAaBBJCEBA9ABwDUlIYADwVvjQsgtJiGAQCohgASAIYADw
E/gA8CP4APAP+E/wgEDQ+MABY/MIIAFJCGAAvcABAEAAABVA/wD+/wC/B0jAaMDzgCAAKPnQAL8D
SMBoAPABAAAo+dFwRwAAAAAVQAC1//fr/xBIQGgQSQhAQPQAcA1JSGAFIAhg//ff/wvgCUhAaAlJ
CEBA9ABwBklIYAUgCGD/99L/A0gAaQDwAQAAKO3RAL0AAAAAFUD/AP7/ALX/98P/DkhAaSD0wCAM
SUhhCEZAaAtJCEAJSUhgBiAIYP/3s/8GSEBoBkkIQED0AHADSUhgBSAIYP/3p/8AvQAAFUD/AP7/
ELUC8I37HyAA8AL4EL0AAADwHwIBIZFAAkpDEUL4IxBwRwAAgOIA4ADwHwIBIZFAQhGSAALx4CLC
+AARcEcAAEDqAQIBSxpgcEcAAAjtAOC/80+PBkgAaAD04GAFSQhDAB0DSQhgv/NPjwC//ucAAAzt
AOAAAPoFcLUA8E/8BkYAJRDgVvglQOCyAPBd+8TzByAA8Fn7xPMHQADwVfsgDgDwUvttHEAt7NNw
vT61akYBqQKoAfAa+wAkCOACmVH4JAAAmQhgAJgAHQCQZBwBmLTrkA/y0wKgAfAk+z69AAAjJGFj
awAAABC1BSAD8Nb4EL0QtTUgA/DR+BC9ELUAIw5MI2BP8ABzGGjA8wAxwPMAIiG5AyNP8IBE42cD
4AAjT/CARONnKrnDskP0gHMDTCNgAuAAIwFMI2AQvcABAEBwtQRGDUZF6kQAxrIksQEiACERSAPw
xPglsQEiACEPSAPwvvgAIwIiMUYNSAPww/g8uQxICEmIZQEiEUYGSAPwr/g9uQdIBUmIZQEiEUYD
SAPwpvhwvQAAAAAUQAAQFEAoAABABAAAgBC1BEYBISBGAPAS+BC9cLUFRgAkCOABIKBAKEAQsSBG
//fu/2AcxLIZLPTbcL0AAHC1BEYNRhksetLf6ATwDRklMT1JVWFteoyeqrzI1OD08vHw7+7t7AAV
IilGlUgD8Gv4EiIpRpNICDAD8GX4HuEWIilGj0gD8F/4EiIpRo1ICDAD8Fn4EuECIilGiUgD8FP4
ACIpRodICDAD8E34BuEDIilGg0gD8Ef4ASIpRoFICDAD8EH4+uAEIilGfUgD8Dv4AiIpRntICDAD
8DX47uABIilGd0gD8C/4ESIpRnVICDAD8Cn44uAFIilGcUgD8CP4CiIpRm9ICDAD8B341uAIIilG
a0gD8Bf4CSIpRmlICDAD8BH4yuAJIilGZUgD8Av4DCIpRmNISDAD8AX4vuC84AoiKUZfSALw/v8T
IilGXEgIMALw+P8UIilGWUgIMALw8v+r4AsiKUZWSALw7P8LIilGU0gIMALw5v8QIilGUEgIMALw
4P+Z4AwiKUZNSALw2v8AIilGSkgAHwLw1P+N4AwiKUZHSAAfAvDN/w0iKUZESALwyP8GIilGQUgI
MALwwv974A4iKUY+SALwvP8HIilGO0gIMALwtv9v4A8iKUY4SALwsP8MIilGNUgIMALwqv9j4BAi
KUYySALwpP8IIilGL0gIMALwnv9X4BEiKUYsSALwmP8EIilGKUgIMALwkv9L4EPgNeAo4CHgGuAT
4Azg/+cSIilGIkgC8IT/BSIpRh9ICDAC8H7/N+ATIilGHEgC8Hj/MeAUIilGGUgC8HL/K+AAIilG
FkgC8Gz/JeAGIilGE0gC8Gb/H+AXIilGEEgC8GD/DiIpRg1ICDAC8Fr/E+AFIilGCkh0MALwU/8H
IilGB0h0MALwTf8G4BkiKUYDSALwR/8A4AC/AL9wvQAADAAAQHC1BUYMRhWxAS0e0RbgDLEBIADg
ACABRhUiDUgC8DD/ASwK2xQjBCIhRhAHAvAz/x4iASEGSALwI/8H4ATwAQEWIgNIAvAc/wDgAL8A
v3C9BAAAQBC1hrAERhQiCEkBqP/3MvsBqQdIAPBy/iJGACEESADwA/8A8BX+BrAQvQAA3F4AIAAA
FkAO4AC/CEmJacHzQBEAKfnREPgBKwRJCWhi8wcBAkoRYAF4ACnt0XBHABABQAC1A0YYRv/35f8K
IADwEvkAvwNIgGnA88AAACj50QC9ABABQAAhBUrRYQkhEWIRRkhiASFRYdFhcEcAAAAgAEAAIAFJ
yGFwRwAgAEBP8IBAwGgg8AQAAB1P8IBByGAIRkBpIPABAEAcSGEIRsBoIPQAAAD1AADIYAhGQGkg
9IBAAPWAQEhhCEbAaCDwQABAMMhgCEbAaCD0ABAA9QAQyGAIRsBoIPSAAAD1gADIYAZIQGtA9ABA
BElIYwRIwGgg8H5gAknIYHBHAFABQABgAEBwR3BHELUfIP/3EP0fIP/3Gf1FSABoIPSAEAD1gBBD
ScH4qAFC9uBg//eU/wC///cL+wAo+9A+SABoIPCAAIAwT/CAQcH4gAA5SABoIPAgACAwwfiAAAhG
AGgg8A8ACDAIYAhGAGgg8IBQAPGAUAhgCEYAaCDw8AAgMAhgCEYAaCDwAFAA8QBQCGAIRkBoIPSA
QAD1gEBIYAC/JkgAaMDzwBAAKPnQJEhAbiDwCAAIMCFJSGYIRkBuIPAQABAwSGYIRgBpIPB/AAhh
CEYAaSD0flAIYRlIgGog9EAwF0mIYghGQGsg9EAwSGMTSIAwAGgg9EAwEEnB+IAAEUj/9yr/AL//
96H6ACj70AdINDgAaG/zCABDMAVJwfh0AQHwtv8ISAHwDfkQvagBEEAAABBAgAAAQMgBAEAAYABA
AFABQACfJAABSgAgAL8FSIBpwPMAEAAo+dECSABowLJwRwAAABABQAC/BkmJacHzQBEAKfnRA0kJ
aGDzBwEBShFgcEcAEAFAOLUAJATg//fd/w34BABkHAQs+NOd+AMAAAad+AIQQOoBQJ34ARBA6gEg
nfgAEAhDOUlIYAAkBOD/98T/DfgEAGQcBCz40534AwAABp34AhBA6gFAnfgBEEDqASCd+AAQCEMt
SYhgACQE4P/3q/8N+AQAZBwELPjTnfgDAAAGnfgCEEDqAUCd+AEQQOoBIJ34ABAIQyBJyGAAJATg
//eS/w34BABkHAQs+NOd+AMAAAad+AIQQOoBQJ34ARBA6gEgnfgAEAhDFEnIYQhGAGgg8AEAQBwI
YE72XnEPSABoYfMbMA1JCGAIRgBoIPSAcAD1gHAIYAC/CEgAawDwAQAAKPnQAL8ESABrAPABAAAo
+dECoP/3S/44vQBwAUAjJGFjawAAAPC1wbAA8ET4Bkb/90n/BEYboP/3Of4AJQTg//dB/w34BQBt
HGAchUL30//3Of+N+AAB//c1/434AQGd+AABnfgBEUDqASc6RmEcaEb/93r5iLG28YBvBdJiHGlG
MEb/97/6BOBiHGlGMEb/90X6BKD/9wr+AuAEoP/3Bv5BsPC9IyRhY2sAAAAjJG5hawAAADi1//cF
/434AAD/9wH/jfgBAP/3/f6N+AIA//f5/o34AwCd+AMAAAad+AIQQOoBQJ34ARBA6gEgnfgAEEDq
AQQgRji98LUMRtcAt/vz8c0JoevFEU8cfgiHayfwEAeHY4drJ/AgB4djR2pl8w8HR2KHambzBQeH
YsdqJ/AQBxA3x2LHaifwYAdgN8diR2sn8P8HR2NHayf0f0cH9YB3R2ME8AEHX7EHayf0gEcH9YBH
B2MHayf0AEcH9QBHB2MHayfwAQd/HAdjB2sn9IB3B/WAdwdjB2sn9AB3B/UAdwdj8L0AAA61akYB
qQKoAPDg/QCYAGgdSUhgAJgAHQCQAJgAaIhgAJgAHQCQAJgAaMhgAJgAHQCQAJgAaMhhCEYAaCDw
AQBAHAhgTvZecQ9IAGhh8xswDUkIYAhGAGgg9IBwAPWAcAhgAL8ISABrAPABAAAo+dAAvwVIAGsA
8AEAACj50QOgAPBQ/g69AAAAcAFAIyRhY2sAAAA+tWpGAakCqADwlv0AJAzgApkB6wQgT/SAcgCZ
//fc+QCYAPWAcACQZBwBmLTrEC/u0wKgAPAs/j69AAAjJGFjawAAAC3p8EOFsE/wAAj/9zf9ACfw
4vZIgGnA8wAQKLnzSABoA6nIVXgcx7IHLwPQAPDm/QAoetAA8OL9xLL/9x/9ACfrSABowPMAICi5
T/AAcABowPMANRLg5kgAaCD0gHBP8IBBwfjAAUgRAGjA8wA1CEbQ+MABQPSAcMH4wAHdoQOo//d2
+Bi526AA8CL9QLEBLQbRFLn/947+ouL/94//n+LWoQOo//dk+Bi51KAA8BD9QLEBLQbRFLn/9+r9
kOL/9zP/jeLPoQOo//dS+Bi5zaAA8P78QLEBLQbRFLn/92D6fuL/93f6e+LIoQOo//dA+Bi5xqAA
8Oz8QLEBLQbRFLn/9076bOL/92X6aeLBoQOo//cu+Bi5v6AA8Nr8SLEBLQfRHLn/9zb4WuJf4v/3
hvhW4rqhA6j/9xv4ELH+9+z/TuK4oQOo//cT+Bi5taAA8L/8QLEBLALRtKAA8Hf9//cL+gC//uez
oQOo//cB+Bi5sKAA8K38OLEBLALRq6AA8GX9/vdX/SvirKEDqP738P8YuaqgAPCc/GCxAS0K0f/3
rfgcuaGg//dJ/Bnin6AA8E39FeKjoQOo/vfa/xi5oaAA8Ib8GLMBLSHRpLn/9zr+Bka28YBvBtIw
Rv/3+/iToP/3LPwG4KbxgGAA8K/7j6D/9yT89OFqRgGpAqgA8Hr8Apj/9+f4iaAA8CD96OGPoQOo
/vet/xi5jKAA8Fn8uLEBLRXRRLn/9w3+Bkb/91D4fqD/9wP80+FqRgGpAqgA8Fn8Apj/90T4eKAA
8P/8x+GAoQOo/veM/xi5fqAA8Dj8ULFP8AAIHLlwoP/35/u34W6gAPDr/LPheKEDqP73eP8YuXag
APAk/NixAS0Z0Vy5//fY/QZG//fV/YFGxvgAkGKg//fL+5vhakYBqQKoAPAh/ACYAGgCmQhgXKAA
8Mb8juFooQOo/vdT/xi5ZaAA8P/7GLMBLSHRnLn/97P9gUbZ+ABg8LL/98P8xvMHIP/3v/zG8wdA
//e7/DAO//e4/G7hakYBqQKoAPCE/AKYAGgAmQhgRaAA8Jn8YeFToQOo/vcm/xi5UaAA8NL76LEB
LRvRbLn/9478Bkb/94v8gUZG6gkg//dc+Dig//d3+0fhakYBqQKoAPDN+wCZCIj/90/4MqAA8HL8
OuFCoQOo/vf//hi5P6AA8Kv7yLEBLRfRTLn+983/Bkbwsv/3cfwwCv/3bvwk4WpGAakCqADwOvz+
977/AJkIgCCgAPBP/BfhMqEDqP733P4YuTCgAPCI+5ixAS0I0Ry5LqD/9zf7B+EsoADwO/wD4Ry5
LKD/9y77/uAqoADwMvz64FPgABABQMABAEAjJHUyZnNoACMkdTJlZnMAIyRmc2gydQAjJHJhbTJ1
ACMkZWZzMnUAIyRzYmF1ZAAjJHJlc2V0ACMkYWNrAAAAIyRqMmZzaAAjJGNoZXJhACMkc3RlcmEA
IyRiM2VyYQAjJGhvbGRkACMkcmVnd3IAIyRyZWdyZAAjJGZzaHN0ACMkZnNoZ3QAIyRzdGF0ZQAj
JHVsawAAACMkbGNrAAAAWqEDqP73av4YuVigAPAW+/CzAS1Q0agHwGgg8IAAgDCpB8hgU0gAaCD0
f0AA9YBwUEkIYAhGgGgg8AMAgByIYAhGgGgg8AQAiGAIRgBoIPAIAAgwCGAIRgBoIPABAEAcCGAA
v0NIgGjA84AAACj50EBIAGgg8AEAPkkIYAhGAGkg9IBwQPSAUAKQACA6SQDgEuAIYAQiAqnQBf73
Mf//93f4JLmv8hAQ//d++gPgr/IYEADwgftJ4DChA6j+9w7+GLkuoADwuvrYsQEtGdEAIClJCGBo
BgBoIPSAUAKQBCICqdAF/vcM///3Uvgkua/yWBD/91n6A+Cv8mQQAPBc+yTgIKEDqP736f0YuR2g
APCV+pixlblP9ABw/vcn//73o/7/9zX4JLmv8pQQ//c8+gzgr/KcEADwP/sH4By5E6D/9zL6AuAR
oADwNvu48QEPAtEQSP/3Ovr+97L9ALEA4A3lAL8FsL3o8IMAACMkZnNoZWMAAHAAQMABAEAjJGxv
Y2trACMkdW5sY2sAIyRuYWsAAAAAHE4OELUAvwVIAPCV+ADwEAAAKPjQECEBSADwRfgQvQAAFkAQ
tQC//vfe/wDwAQAAKPnREL0QtQYg//e7+RC9AuAIyBIfCMEAKvrRcEdwRwAgAeABwRIfACr70XBH
AAAQSQlo0fhAIg5JCWgB9RFxQfgiAAtJCWjR+EAiCUkJaNH4QBJJHAdLG2jD+EASQPJ/IYpCBNMB
IQJKEmjC+EAScEcAAAAAACDCawpDwmNwRxC1BEYLRiBGAPBB+Jh4ACFB6kAAWXgIQ9l4QOrBABl5
QOoBIFl5QOqBAJl8QOoBQCBhmHkABtl5QOqBURiJQB7A8wgAQeoAMFmJSR7B8wgBCEMZe0DqQXBZ
e0DqgXBZfEDqwXAgYtl7IEYA8FH4GXwgRgDwPPgZeCBGAPBB+Jl7IEYA8Bf4EL0BRshrcEcAtQJG
EGtA8AEAEGMAvxBrAPABAAAo+tEQRgDwFvgQRgDwCPgAvYJrCkOCY3BHwmoKYHBHAWtB8AIBAWMA
vwFrwfNAAQAp+tFwRwFrQfAEAQFjAL8Ba8HzgAEAKfrRcEcCayL0f0ICYwJrQuoBIgJjcEcCbCLw
/wICZAJsCkMCZHBHAmsi9H8CAmMCa0LqAUICY3BHAUZIa8DzwFBwR4FiQmJwR8FicEcAAHC1AkYM
Rv8l/yMJ4BL4AQuA6gUBBkhAXIDqAwUFSENcIB6k8QEGtLLw0UPqBSBwvdxcACDcXQAgELUERgy5
ASAA4AAgAUYAIP73Bv8QvQAAELUPSP73O/8OSA9JCGAPSAhhT/SoIEhhDUgIZA1JCkhBZAAhCEb/
93/4APBV+LD1AH8D0E/0AHAA8IT4EL0AAAAQIAAACm8FAGAAQEeVCgBkSCkBlNL/AfC1hbAERg9G
FUb/98z+FCIZSWhG/vfA++gcgLKt+AgAaUYWSP/3/P4CIgAhE0j/943/xPMHQRFI//eL/8TzByEO
SP/3hv/hsgxI//eC/wAmCuAAvwlI//d0/wAo+tG5XQZI//d2/3YcrkLy0//3gv7/95D+BbDwvRhf
ACAAABZAELX+927+BAL+92b+BEMgRhC9AAAQtYawBEb/94T+FCIQSQGo/vd4+wGpD0j/97j+ICIA
IQxI//dJ/8TzB0EKSP/3R//E8wchB0j/90L/4bIFSP/3Pv//903+//db/gawEL0AAARfACAAABZA
ELWGsARG//dY/hQiDUkBqP73TPsBqQxI//eM/gEiACEJSP/3Hf/hsgdI//cc/yEKBUj/9xj///cn
/v/3Nf4GsBC9AAAsXwAgAAAWQBRIgGlA8BAAEkmIYRJIAGgSSQhDEEkIYAhGAGgg9P5gCGAIRgBo
IPQAAAhgCEYAaCDwBAAIYAgdAGgg8AIACR0IYARIgGlD9ghBCEMBSYhhcEcAAAAAGEAACBhAAwAA
AQdICEkIYAhIAGgJHQhgBUgQOABoQPAEQAJJEDkIYHBHGAAIYBALGEAAAAAgELUDRhJIAGgA9aBx
C7kAIBC9ACIK4AHxOABQ+CIAmEIC0VAcwLLz51AcwrKR+OsAkELw3JH460AB8TgAQPgkMBH46w9A
HAhwEfjrCeDnAAAAAAAgcLUERgAlBkkJaET2rHIRREhoIUb+99/6ALkBJShGcL0AAAAgMLUDRgZM
JGhE9qx1LERgaIRpHGDEaQxgAPFABBRgML0AAAAgLenwXwVGiEaRRppGT/ABCypIAGhE9uhBRBgo
SABoRPascUcYFbG48QAPAdFP8AALfWDH+AiALkY1RjBpyfgAAHBpyvgAACBoQBwgYBAoAdMAIADg
IGggYCFoCEYB60ACAuvBAiAdQPgiEDFpIGgA60ACAuvAAiAdAOuCAEFgcWkgaADrQAIC68ACIB0A
64IAgWB5aCJoAutCAwPrwgMiHQLrgwIC8QwA/vd/+gAgeHAbIDhwWEa96PCfAAAAAAAg8LUERg9G
FUYeRqqyACM5RoIgAfCv+rKyACMhRgEgAfBD+vC9ACAESQloAfWAQZH4rB8BsQEgcEcAAAAgMLUD
RgtMJGhE9qx1YBlEaKRphWisYURo5GmFaOxhhGikaRxghGjkaQxghGhANBRgML0AAAAAACAt6fBB
BUYUSABoRPboQUYYEUgAaET2rHFEGClGoGj+9yr6MWgB60ECAuvBAjEdAeuCAQHxHAApRv73HfoE
IGBwACAgcGdo1+kEIzhGoWj/953/vejwgQAAAAAAIC3p8EEGRg9GkEYm8IAEE0gAaAD1oHUQLB7a
7LEG8IAAaLFgHgXxDAFR+CAAoLFgHlH4ICBBRjhGkEe96PCBYB4F8SABUfggADCxYB5R+CAgQUY4
RpBH8OcCIO7nAAAAIDC1AkYi8IAAGUsbaAP1oHEC8IADq7FP8P80AOtABQHxWAND+CVAACQA60AF
A+uFA1xgAOtABQHxWAMD64UDnGAU4E/w/zQA60AFAfGgA0P4JUAAJADrQAUD64UDXGAA60AFAfGg
AwPrhQOcYDC9AAAAAAAgcLUySABoAPWgdKBo4LOU+OgA/yg10KBowGiU+OgQUPghAHCzoGjAaJT4
6BBQ+CEAgGgwswAlGeCgaMBolPjoEFD4IQCAaFD4JQDAaGCxoGjAaJT46BBQ+CEAgGhQ+CUAACHC
aAUgkEdoHMWyoGjAaJT46BBQ+CEAgGhQ+CUAACja0f8ghPjoAAAlDOAAIQTxDABA+CUQaRzJskHw
gAAA8KX5aBzFsgUt8NsAJQvgACEE8SAAQPglEGkcAfB/AADwlfloHMWyBS3x23C9AAAAIBC1BEYy
SCDwAwAySQhgRPa4cTBIAGj+9w75LUkh8AMA//cM/CxIwGhA8EAAKknIYADw6PgA8HL7J0iAaEDw
JgAlSYhgCEaAaEDwAQCIYAhGwGhA8AgAyGAIRsBoIPRwUMhgCEbAaED0EFDIYAhGgGlA8AYAiGEB
8C36ECAA8A77APDq+v/3jv0TSEBpEklJaQhDEUlIYQhGQGhJaAhDDklIYAxJCWgB9aBwACFBcZD4
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
H/8FqQRI/ffP/wWYwLIGsBC9AADwXgAgAAAWQDC1ASQE+gLzBGicQwH6AvUsQwRgML1wtQEllUBt
HgX6A/QFaKVDAfoD9jVDBWBwvQDBgUABwIBBAcCAQQDBgUABwIBBAMGBQADBgUABwIBBAcCAQQDB
gUAAwYFAAcCAQQDBgUABwIBBAcCAQQDBgUABwIBBAMGBQADBgUABwIBBAMGBQAHAgEEBwIBBAMGB
QADBgUABwIBBAcCAQQDBgUABwIBBAMGBQADBgUABwIBBAcCAQQDBgUAAwYFAAcCAQQDBgUABwIBB
AcCAQQDBgUAAwYFAAcCAQQHAgEEAwYFAAcCAQQDBgUAAwYFAAcCAQQDBgUABwIBBAcCAQQDBgUAB
wIBBAMGBQADBgUABwIBBAcCAQQDBgUAAwYFAAcCAQQDBgUABwIBBAcCAQQDBgUAAwMEBwwMCwsYG
B8cFxcQEzAwNzQ/Pzg4KyssLyQkIyNgYGdkb29oaHt7fH90dHNwU1NUV1xcW1tISE9MR0dAQ8DAx
8TPz8jI29vc39TU09Dz8/T3/Pz7++jo7+zn5+Dgo6Okp6ysq6u4uL+8t7ews5CQl5Sfn5iYi4uMj
4SEg4KBgYaFjo6JiZqanZ6VlZKRsrK1tr29urqpqa6tpqahoeLi5ebt7erq+fn+/fb28fLR0dbV3
t7Z2crKzc7FxcLBQkJFRk1NSkpZWV5dVlZRUnFxdnV+fnl5amptbmVlYmIhISYlLi4pKTo6PT41N
TIxEhIVFh0dGhoJCQ4NBgYBAAAAAAAcABwABAAEAAAEQAAAAAAAAAAAABwACAAEAAQAAARAAAAAA
AAAAAAAHAAEAAwABAAABEAAAAAAAAAAAAAcAAQAAAAEAAAEQAAAAAAAAAAAABwABAAIAAQAAARAA
AAAAAGBfACAAAAAgXAAAAAw7ACC8XwAgXAAAIAwQAAAcOwAgAAAAAAAAAABCb290bG9hZGVyAAAA
AAAAAAAAAEJvb3Rsb2FkZXIgSW50ZXJmYWNlAAAAAAAAAAAAAAAASW5nY2hpcHMgVVNCIERldmlj
ZQAAAAAAAAAAAAAAAAA="""]
    if (flashtype < 0) or (flashtype >= len(loaders)):
        raise Exception(f'unsupported external flash type: {flashtype}')
    bin = base64.b64decode(''.join(loaders[flashtype].splitlines()))

    entry_addr = 0x20002000
    print('downloading external flash bootloader @ {:#x} ...'.format(entry_addr))
    if not intf.send_file(d.dev, entry_addr, bin):
        return False

    intf.direct_jump(d.dev, entry_addr)
    time.sleep(0.2)
    d.dev.read_all()
    return True

def prepare_loader(intf, d: device, config) -> bool:
    use_ram = False
    use_ext = False
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

    if use_ext and (not isinstance(intf, intf_uart)):
        raise Exception(f'Not supported: download to external flash through {intf}')

    if not use_ext: return True

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

    if config.getboolean('options', 'writeprotection.enabled', fallback=False):
        intf.set_flash_write_protection(d.dev)

    if config.getboolean('options', 'protection.enabled', fallback=False):
        if not intf.lock(d.dev):
            return 6

    if config.getboolean('options', 'launch'):
        intf.launch_app(d.dev)

    return do_test(d.dev, config)