import serial
import argparse
import sys
import time
import icsdw

def open_serial(config):
    ser = serial.Serial()
    ser.port = icsdw.get_port_name(config['port'])
    ser.baudrate = icsdw.DEF_BAUD
    ser.parity = icsdw.get_parity(config['parity'])
    ser.stopbits = int(config['stopbits'])
    ser.bytesize = int(config['databits'])
    ser.timeout = None
    ser.open()
    if not ser.is_open:
        raise Exception("can't open serial")
    return ser

def do_run_918(ser, FLAGS):
    import icsdw918

    def read_page(ser: serial.Serial, addr):
        ser.write(icsdw918.READ_PAGE + addr.to_bytes(4, 'little'))
        return ser.read(icsdw918.PAGE_SIZE)

    if not FLAGS.go:
        if not icsdw918.wait_handshaking(ser, FLAGS.timeout):
            print("handshaking timeout")
            return 1
    else:
        ser.reset_input_buffer()

    if icsdw918.is_locked(ser):
        print("flash locked")
        return 3

    if FLAGS.baud != icsdw.DEF_BAUD:
        print('baud -> {}'.format(FLAGS.baud))
        if not icsdw.set_baud(ser, FLAGS.baud):
            return 3
        time.sleep(0.1)
        ser.baudrate = FLAGS.baud

    with open(FLAGS.file_name, 'wb') as f:
        for i in range(FLAGS.page_no):
            icsdw.printProgressBar(i, FLAGS.page_no)
            addr = FLAGS.start_addr + i * icsdw918.PAGE_SIZE
            f.write(read_page(ser, addr))
        icsdw.printProgressBar(FLAGS.page_no, FLAGS.page_no)

    return 0

def do_run_916(ser, FLAGS):
    import icsdw916

    def read_page(ser: serial.Serial, addr):
        ser.write(icsdw916.READ_PAGE + addr.to_bytes(4, 'little'))
        return ser.read(icsdw916.PAGE_SIZE)

    if not FLAGS.go:
        if not icsdw916.wait_handshaking(ser, FLAGS.timeout):
            print("handshaking timeout")
            return 1
    else:
        ser.reset_input_buffer()

    if FLAGS.baud != icsdw.DEF_BAUD:
        print('baud -> {}'.format(FLAGS.baud))
        if not icsdw.set_baud(ser, FLAGS.baud):
            return 3
        time.sleep(0.1)
        ser.baudrate = FLAGS.baud

    with open(FLAGS.file_name, 'wb') as f:
        for i in range(FLAGS.page_no):
            icsdw.printProgressBar(i, FLAGS.page_no)
            addr = FLAGS.start_addr + i * icsdw916.PAGE_SIZE
            f.write(read_page(ser, addr))
        icsdw.printProgressBar(FLAGS.page_no, FLAGS.page_no)

    return -1

def run(FLAGS):
    config = {
        'port': FLAGS.port,
        'baudrate': icsdw.DEF_BAUD,
        'parity': '',
        'stopbits': 1,
        'databits': 8
    }
    ser = open_serial(config)

    if ser == None:
        return 1

    ser.timeout = FLAGS.timeout

    try:
        if FLAGS.family == 'ing918':
            r = do_run_918(ser, FLAGS)
        elif FLAGS.family == 'ing916':
            r = do_run_916(ser, FLAGS)
        else:
            raise Exception('unknown chip family: ' + FLAGS.family)
    finally:
        ser.close()
    return r

def auto_int(x):
    return int(x, 0)

if __name__ == '__main__':

    parser = argparse.ArgumentParser()
    parser.add_argument(
        'port',
        type=str,
        help='COM port')

    parser.add_argument(
        'start_addr',
        type=auto_int,
        help='flash start address.')

    parser.add_argument(
        'page_no',
        type=int,
        help='number of pages.')

    parser.add_argument(
        'file_name',
        type=str,
        help='file name.')

    parser.add_argument(
        '--baud',
        type=int,
        default=115200,
        help='baud rate.')

    parser.add_argument(
        '--go',
        type=bool,
        default=False,
        nargs='?',
        const=True,
        help='skip handshaking.')

    parser.add_argument(
        '--timeout',
        type=int,
        default=5,
        help='timeout.')

    parser.add_argument(
        '--family',
        type=str,
        default='ing918',
        help='chip family.')

    FLAGS, unparsed = parser.parse_known_args()

    try:
        r = run(FLAGS)
        sys.exit(r)
    except Exception as e:
        print("Exception: ", e)
        print("")
        print(icsdw.format_exception(e))
        time.sleep(5)
        sys.exit(1)
