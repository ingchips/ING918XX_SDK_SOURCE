from types import ModuleType
import serial
import time
import icsdw
from icsdw import calc_crc_16, printProgressBar, call_on_batch, call_on_file, do_test

PAGE_SIZE = 256
SECTOR_SIZE = 4 * 1024

BOOT_HELLO    = b'UartBurnStart916\n'
ACK           = b'#$ack\n'
NACK          = b'#$nak\n'
STATUS_LOCKED   = b'#$lck\n'
STATUS_UNLOCKED = b'#$ulk\n'

RSP_LEN = len(ACK)

SEND_PAGE   = b'#$u2fsh'
READ_PAGE   = b'#$fsh2u'
SEND_RAM_DATA=b'#$u2ram'
ERASE_SECTOR  = b'#$stera'
CMD_LAUNCH  = b'#$j2fsh'
CMD_JUMPD   = b'#$jumpd'
CMD_SET_BAUD= b'#$sbaud'
CMD_FLASH_SET  = b'#$fshst'
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

def launch_app(ser: serial.Serial):
    ser.write(CMD_LAUNCH)

def direct_jump(ser: serial.Serial, addr: int):
    return exec_cmd(ser, CMD_JUMPD + addr.to_bytes(4, 'little')) == ACK

def erase_sector(ser: serial.Serial, addr: int):
    return exec_cmd(ser, ERASE_SECTOR + addr.to_bytes(4, 'little')) == ACK

def unlock(ser: serial.Serial):
    return exec_cmd(ser, CMD_UNLOCK) == ACK

def lock(ser: serial.Serial):
    return exec_cmd(ser, CMD_LOCK) == ACK

def set_baud(ser: serial.Serial, baud: int):
    rsp = exec_cmd(ser, CMD_SET_BAUD + baud.to_bytes(4, 'little'))
    return rsp == ACK

def send_page(ser: serial.Serial, addr: int, data: bytes):
    CMD = SEND_PAGE if addr < icsdw.RAM_BASE_ADDR else SEND_RAM_DATA
    rsp = exec_cmd(ser, CMD + addr.to_bytes(4, 'little') + (len(data) - 1).to_bytes(1, 'little'))
    if rsp != ACK:
        return False
    ser.write(data)
    crc = calc_crc_16(data)
    ser.write(crc.to_bytes(2, 'little'))
    rsp = ser.read(len(ACK))
    return rsp == ACK

def send_sector(ser: serial.Serial, addr: int, data: bytes):
    if (addr < icsdw.RAM_BASE_ADDR) and (not erase_sector(ser, addr)):
        return False

    offset = 0
    while len(data) > offset:
        seg = min(len(data) - offset, PAGE_SIZE)

        if not send_page(ser, addr + offset, data[offset:offset + seg]):
            return False

        offset += seg

    return True

def send_file(ser: serial.Serial, addr: int, data: bytes):
    offset = 0
    while len(data) > offset:
        seg = min(len(data) - offset, SECTOR_SIZE)

        if not send_sector(ser, addr + offset, data[offset:offset + seg]):
            return False

        offset += seg

        printProgressBar(offset, len(data))

    return True

def wait_handshaking(ser: serial.Serial, timeout: int):
    import icsdw918
    return icsdw918.wait_handshaking3(ser, timeout, BOOT_HELLO)

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

    if config.getboolean('options', 'ResetReservedFlash'):
        erase_sector(0x2000000)

    if config.getint('uart', 'Baud') != icsdw.DEF_BAUD:
        print('baud -> {}'.format(config.getint('uart', 'Baud')))
        if not set_baud(ser, config.getint('uart', 'Baud')):
            return 3
        time.sleep(0.1)
        ser.baudrate = config.getint('uart', 'Baud')

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
        if not send_file(ser, addr, new_data):
            return 4

    if config.getboolean('options', 'set-entry'):
        entry_addr = int(config.get('options', 'entry'), 0)

        if entry_addr >= icsdw.RAM_BASE_ADDR:
            direct_jump(ser, entry_addr)
            return do_test(ser, config)
        else:
            pass

    if config.getboolean('options', 'protection.enabled'):
        if not lock(ser):
            return 6

    if config.getboolean('options', 'launch'):
        launch_app(ser)

    return do_test(ser, config)