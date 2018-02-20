import serial
import time
import numpy
import threading
import datetime
import sys

def log(msg):
    print('[%s] %s' % (datetime.datetime.now(), msg))

def bytes_to_string(bs):
    return ' '.join(map(lambda b: '%02x' % b, bs))

def calculate_checksum(bs):
    checksum = 0
    for b in bs:
        checksum ^= b
    return checksum

def check_checksum(length, cmd, data, checksum):
    checksum ^= length
    checksum ^= cmd[0]
    checksum ^= cmd[1]
    for byte in data:
        checksum ^= byte
    return (checksum == 0)

CMD_TYPE_MAP = {
    0: 'POLL',
    1: 'SREQ',
    2: 'AREQ',
    3: 'SRSP',
}

class ListeningThread(threading.Thread):
    def __init__(self, s):
        super(ListeningThread, self).__init__()
        self.s = s

    def run(self):
        log('Thread is listening...')

        while True:
            start = self.s.read(1)

            if start[0] != 0xFE:
                log('Ignoring incorrect start byte 0x%02x' % start[0])

            length = self.s.read(1)[0]
            cmd = self.s.read(2)
            data = self.s.read(length)
            checksum = s.read(1)[0]

            cmd_type = CMD_TYPE_MAP[(cmd[0] & 0xE0) >> 5]

            if not check_checksum(length, cmd, data, checksum):
                log('Invalid checksum!!!')

            log('Received command (%s %s): payload = %s' %
                (cmd_type, bytes_to_string(cmd), bytes_to_string(data)))

if sys.argv[1] == 'master':
    DEV = '/dev/cu.wchusbserial401120'
elif sys.argv[1] == 'slave':
    DEV = '/dev/cu.usbserial-A9M9DV3R'
else:
    raise RuntimeError("Invalid device given.")

def send_command(s, cmd):
    log('Sending %s' % bytes_to_string(cmd))

    length = len(cmd) - 2

    s.write(bytes([0xFE]))
    s.write(bytes([length]))
    s.write(cmd)
    s.write(bytes([calculate_checksum(bytes([length]) + cmd)]))

    time.sleep(0.1)

with serial.Serial(DEV, 115200) as s:
    time.sleep(1)

    listener = ListeningThread(s)
    listener.start()

    if len(sys.argv) > 2:
        if sys.argv[2] == 'coord':
            send_command(s, b'\x21\x09\x03\x00\x00\x01\x03')
            send_command(s, b'\x41\x00\x00')

            time.sleep(3)

            send_command(s, b'\x21\x09\x87\x00\x00\x01\x00')
            send_command(s, b'\x2F\x08\x01\x00\x20\x00\x00')
            send_command(s, b'\x2F\x08\x00\x00\x00\x00\x00')
            send_command(s, b'\x2F\x05\x04')

            time.sleep(3)

            send_command(s, b'\x21\x09\x8F\x00\x00\x01\x01')
            send_command(s, b'\x2F\x05\x02')

            time.sleep(1)

            send_command(s, b'\x24\x00\x01\x04\x01\x00\x01\x00\x00\x02\x00\x00\x06\x00\x00\x00\x06\x00')
        elif sys.argv[2] == 'router':
            send_command(s, b'\x21\x09\x03\x00\x00\x01\x03')
            send_command(s, b'\x41\x00\x00')

            time.sleep(3)

            send_command(s, b'\x21\x09\x87\x00\x00\x01\x01')
            send_command(s, b'\x2F\x08\x01\x00\x20\x00\x00')
            send_command(s, b'\x2F\x08\x00\x00\x00\x00\x00')
            send_command(s, b'\x2F\x05\x02')

            time.sleep(5)

            send_command(s, b'\x24\x00\x01\x04\x01\x00\x01\x00\x00\x02\x00\x00\x06\x00\x00\x00\x06\x00')
        else:
            raise RuntimeError("Invalid startup sequence.")

    log('Accepting manual commands')

    for line in sys.stdin:
        tokens = line.split()
        tokens = filter(lambda x: len(x) == 2, tokens)
        bs = bytes(map(lambda x: int(x, 16), tokens))

        send_command(s, bs)

    listener.join()
