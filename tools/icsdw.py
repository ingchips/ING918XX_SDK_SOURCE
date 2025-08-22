from types import ModuleType
import serial
import configparser
import argparse
import os.path
import sys
import time
import usb.core
import usb.util
import usb.backend.libusb1
import datetime
import libusb_package

DEF_BAUD = 115200
RAM_BASE_ADDR = 0x20000000

auchCRCHi= [
           0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,0x40,
           0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
           0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
           0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
           0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
           0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
           0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
           0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
           0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
           0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
           0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
           0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
           0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
           0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
           0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
           0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
         ]

auchCRCLo = [
           0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,0x04,
           0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8,
           0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
           0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3, 0x11, 0xD1, 0xD0, 0x10,
           0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
           0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
           0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C,
           0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26, 0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0,
           0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
           0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
           0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C,
           0xB4, 0x74, 0x75, 0xB5, 0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
           0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54,
           0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98,
           0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
           0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80, 0x40
           ]

def calc_crc_16(PData: bytes):
  uchCRCHi = 0xFF
  uchCRCLo = 0xFF

  for b in PData:
    uIndex = uchCRCHi ^ b
    uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex]
    uchCRCLo = auchCRCLo[uIndex]

  return (uchCRCHi << 8) | uchCRCLo

def load_mod(fn: str):
    import importlib.machinery, importlib.util
    loader = importlib.machinery.SourceFileLoader('script_mod', fn)
    spec = importlib.util.spec_from_loader(loader.name, loader)
    mod = importlib.util.module_from_spec(spec)
    loader.exec_module(mod)
    return mod

def printProgressBar (iteration, total, prefix = '', suffix = '', decimals = 1, length = 60, fill = '█', printEnd = "\r", auto_nl = True):
    percent = ("{0:." + str(decimals) + "f}").format(100 * (iteration / float(total)))
    filledLength = int(length * iteration // total)
    bar = fill * filledLength + '-' * (length - filledLength)
    print(f'\r{prefix} |{bar}| {percent}% {suffix}', end = printEnd)
    if (iteration == total) and auto_nl:
        print()

def get_port_name(name):
    if name.isdigit():
        return 'COM' + name if sys.platform == 'win32' else name
    else:
        return name

def get_parity(cfg):
    if cfg == 'even':
        return serial.PARITY_EVEN
    elif cfg == 'odd':
        return serial.PARITY_ODD
    elif cfg == 'mark':
        return serial.PARITY_MARK
    elif cfg == 'space':
        return serial.PARITY_SPACE
    else:
        return serial.PARITY_NONE

def open_serial(config, port):
    ser = serial.Serial()
    ser.port = port if port != '' else get_port_name(config['port'])
    ser.baudrate = DEF_BAUD
    ser.parity = get_parity(config['parity'])
    ser.stopbits = int(config['stopbits'])
    ser.bytesize = int(config['databits'])
    ser.timeout = None
    ser.open()
    if not ser.is_open:
        raise Exception("can't open serial")
    return ser

def quit(ser: serial.Serial, code: int):
    if ser != None:
        ser.close()
    sys.exit(code)

def call_on_batch(mod: ModuleType, batch: int):
    if mod != None:
        return mod.on_start_run(batch)
    else:
        return False

def call_on_file(mod: ModuleType, batch: int, id: int, data: bytes, user_data: str):
    if mod != None:
        return mod.on_start_bin2(batch, id, data, user_data) if hasattr(mod, 'on_start_bin2') else \
                        mod.on_start_bin(batch, id, data)
    else:
        return False, data

def get_response(ser):
    ser.timeout = 2
    acc = b''
    while True:
        x = ser.read(1)
        if len(x) == 0:
            break
        if (x[0] == 13) or (x[0] == 10):
            if len(acc) > 0:
                break
        acc = acc + x
    return acc.decode()

def test_app_auto(ser):
    while True:
        rsp = get_response(ser)
        if rsp == '':
            print("Tiemout")
            return 99
        print(rsp)
        if rsp.find('+PASS') >= 0:
            return 0
        elif rsp.find('+ERR') >= 0:
            return 1

def test_app_cust(ser, mod):
    return 0

def do_test(ser, config):
    s = config.get('options', 'testing', fallback='')
    if s == '':
        return 0

    print("testing...")
    if s == 'auto':
        return test_app_auto(ser)
    else:
        return test_app_cust(ser, s)

class device(object):
    def __init__(self, port, timeout, config):
        if port.lower().startswith('daplink'):
            raise Exception(f'Port {port} is NOT supported yet.')
        self.dev_type = 1 if port.lower().startswith('usb') else 0
        self.dev = None
        if self.dev_type == 1:
            self.backend = usb.backend.libusb1.get_backend(find_library=libusb_package.find_library)
        self.open(port, timeout, config)

    @staticmethod
    def query_all_active_usb_ports(timeout = 10):
        start_time = datetime.datetime.now()
        backend = usb.backend.libusb1.get_backend(find_library=libusb_package.find_library)
        from icsdw916 import intf_usb
        intf = intf_usb()
        devs = []
        while True:
            for dev in usb.core.find(find_all=True, idVendor=0xffff, idProduct=0xfa2f, backend=backend):
                assert intf.exec_cmd(dev, intf.CMD_HOLD) == intf.ACK[:5]
                usb.util.dispose_resources(dev)
                devs.append(f'USB#VID_%04X#PID_%04X#%02X#%02X#%02X'%(dev.idVendor, dev.idProduct, dev.address, dev.bus, dev.port_number))
                print(devs[-1])
            if len(devs) or (datetime.datetime.now() -start_time).seconds > timeout:
                return devs
            else:
                time.sleep(1)

    def open(self, port, timeout, config):
        if self.dev_type == 0: #uart
            self.dev = open_serial(config, port)
            self.dev.timeout = timeout
        else: #usb
            if port.lower() == 'usb': #pick the first usb device
                self.dev = usb.core.find(idVendor=0xffff, idProduct=0xfa2f, backend=self.backend)
            else:
                nouse,vid,pid,addr,bus,p = port.split('#')
                self.dev = usb.core.find(custom_match = lambda d: d.idProduct==int(pid.split('_')[1],16) and d.idVendor==int(vid.split('_')[1],16) and d.address==int(addr,16), backend=self.backend)

        return self.dev

    def close(self):
        if self.dev_type == 0: #uart
            self.dev.close()
        else: #usb
            usb.util.dispose_resources(self.dev)

def list_jlink():
    import pylink
    jlink = pylink.JLink()
    for x in jlink.connected_emulators():
        print(x)

def run_proj_jlink(config, mod, port, counter, user_data):
    verify = config.getboolean('options', 'verify', fallback=False)
    family = config.get('main', 'family', fallback='ing918')

    serial_no = port[6:] if port.startswith('jlink#') else None

    # some options are not supported
    if config.getboolean('options', 'protection.enabled'):
        print('WARNING: unsupported function: protection')
    if config.getboolean('options', 'set-entry'):
        if family == 'ing918':
            print('WARNING: unsupported function: set entry for ing918')

    def jlink_on_progress(action, progress_string, percentage):
        printProgressBar(percentage, 100, action.decode(), auto_nl = False)

    import pylink
    jlink = pylink.JLink()
    jlink.open(serial_no=serial_no)
    jlink.set_tif(pylink.enums.JLinkInterfaces.SWD)
    jlink.disable_dialog_boxes()

    global SCRIPT_MOD
    SCRIPT_MOD = mod
    batch_counter = counter
    if batch_counter < 0:
        batch_counter = config.getint('options', 'batch.current')

    if call_on_batch(SCRIPT_MOD, batch_counter):
        return 10

    if family == 'ing918':
        jlink.connect('ING9188xx')
    elif family == 'ing916':
        jlink.connect('ING9168xx')
    else:
        raise ValueError("Invalid device type")

    if not jlink.target_connected():
        raise ConnectionError("Failed to connect to target")

    for i in range(6):
        bcfg = dict(config.items('bin-' + str(i)))
        if bcfg['checked'] != '1':
            continue

        addr = int(bcfg['address'])

        if addr >= RAM_BASE_ADDR:
            raise Exception('unsupported function: download to RAM')

        print('downloading {} @ {:#x} ...'.format(bcfg['filename'], addr))
        data = open(bcfg['filename'], "rb").read()
        abort, new_data = call_on_file(SCRIPT_MOD, batch_counter, i + 1, data, user_data)
        if abort:
            return 10

        jlink.flash(new_data, addr, on_progress=jlink_on_progress)
        print()

    if config.getboolean('options', 'launch'):
        jlink.reset()

    jlink.close()
    return 0

def run_proj(proj: str, go = False, port = '', timeout = 5, counter = -1, user_data = ''):
    mod = None
    config = configparser.ConfigParser()
    config.read(proj)

    if os.path.dirname(proj) != '':
        os.chdir(os.path.dirname(proj))

    if config.getboolean('options', 'usescript'):
        mod = load_mod(config.get('options', 'script'))

    port_cfg = dict(config.items('uart'))
    if port == '':
        port = port_cfg['port']

    if port.lower().startswith('jlink'):
        return run_proj_jlink(config, mod, port, counter, user_data)

    d = device(port, timeout, port_cfg)

    if d.dev is None:
        raise Exception('port not available')

    try:
        family = config.get('main', 'family', fallback='ing918')
        if family == 'ing918':
            import icsdw918
            r = icsdw918.do_run(mod, d.dev, config, go, timeout, counter, user_data)
        elif family == 'ing916':
            import icsdw916
            r = icsdw916.do_run(mod, d, config, go, timeout, counter, user_data)
        elif family == 'ing920':
            import icsdw916
            icsdw916.BOOT_HELLO = b'UartBurnStart920\n'
            r = icsdw916.do_run(mod, d, config, go, timeout, counter, user_data)
        else:
            raise Exception('unknown chip family: ' + family)
    finally:
        d.close()
    return r

def format_exception(e):
    import traceback
    exception_list = traceback.format_stack()
    exception_list = exception_list[:-2]
    exception_list.extend(traceback.format_tb(sys.exc_info()[2]))
    exception_list.extend(traceback.format_exception_only(sys.exc_info()[0], sys.exc_info()[1]))

    exception_str = "Traceback (most recent call last):\n"
    exception_str += "".join(exception_list)
    # Removing the last \n
    exception_str = exception_str[:-1]

    return exception_str

def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        'proj',
        type=str,
        help='Location of project file (.ini).')

    parser.add_argument(
        '--go',
        type=bool,
        default=False,
        nargs='?',
        const=True,
        help='Skip handshaking.')

    parser.add_argument(
        '--batch',
        type=bool,
        default=False,
        nargs='?',
        const=True,
        help='Batch operation.')

    parser.add_argument(
        '--port',
        type=str,
        default='',
        help='Override port.')

    parser.add_argument(
        '--timeout',
        type=float,
        default=5.0,
        help='UART timeout.')

    parser.add_argument(
        '--counter',
        type=int,
        default=-1,
        help='Batch counter.')

    # Processing of user data is up to customers
    parser.add_argument(
        '--user_data',
        type=str,
        default='',
        help='User data.')

    return parser.parse_known_args()

if __name__ == '__main__':

    FLAGS, unparsed = parse_args()

    if FLAGS.proj == 'list-usb':
        device.query_all_active_usb_ports()
        sys.exit(0)

    if FLAGS.proj == 'list-jlink':
        list_jlink()
        sys.exit(0)

    try:
        r = run_proj(FLAGS.proj, FLAGS.go, FLAGS.port, FLAGS.timeout, FLAGS.counter, FLAGS.user_data)
        sys.exit(r)
    except Exception as e:
        print("Exception: ", e)
        print("")
        print(format_exception(e))
        time.sleep(5)
        sys.exit(1)
