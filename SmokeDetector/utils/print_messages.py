import serial
import time
import numpy
import threading
import datetime
import sys

def log(msg):
    sys.stdout.write('[%s] %s\n' % (datetime.datetime.now(), msg))
    sys.stdout.flush()

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

            if cmd[0] == 0x44 and cmd[1] == 0x81:
                msg_len = data[16]
                int_volt = 256 * data[17] + data[18]

                log('Received voltage reading: %.4f V' % (1.0 * int_volt / 4096.0))

if sys.argv[1] == 'slave':
    DEV = '/dev/cu.wchusbserial401120'
elif sys.argv[1] == 'master':
    DEV = '/dev/cu.usbserial-A9M9DV3R'
else:
    DEV = sys.argv[1]

def send_command(s, cmd):
    log('Sending %s' % bytes_to_string(cmd))

    length = len(cmd) - 2

    s.write(bytes([0xFE]))
    s.write(bytes([length]))
    s.write(cmd)
    s.write(bytes([calculate_checksum(bytes([length]) + cmd)]))

    time.sleep(0.1)

PAN_ID = 0xBEEF

with serial.Serial(DEV, 115200) as s:
    time.sleep(1)

    listener = ListeningThread(s)
    listener.start()

    if len(sys.argv) > 2:
        should_reset = (len(sys.argv) > 3 and sys.argv[3] == 'reset')

        if sys.argv[2] == 'coord':
            if should_reset:
                log('Setting reset...')
                send_command(s, b'\x21\x09\x03\x00\x00\x01\x03')

            log('Restarting...')
            send_command(s, b'\x41\x00\x00')
            time.sleep(5)

            if should_reset:
                log('Setting role, channel masks, and PAN ID...')
                send_command(s, b'\x21\x09\x87\x00\x00\x01\x00')
                send_command(s, b'\x2F\x08\x01\x00\x20\x00\x00')
                send_command(s, b'\x2F\x08\x00\x00\x00\x00\x00')
                send_command(s, b'\x21\x09\x83\x00\x00\x02\xEF\xBE')

            log('Registering...')
            send_command(s, b'\x24\x00\x01\x04\x01\x00\x01\x00\x00\x02\x00\x00\x06\x00\x02\x00\x00\x06\x00')

            log('Starting up...')
            send_command(s, b'\x25\x40\x00\x00')
            time.sleep(3)
            send_command(s, b'\x2f\x05\x02')
        elif sys.argv[2] == 'router':
            log('Setting reset bit and restarting...')
            send_command(s, b'\x21\x09\x03\x00\x00\x01\x03')
            send_command(s, b'\x41\x00\x00')

            time.sleep(3)

            log('Setting role...')
            send_command(s, b'\x21\x09\x87\x00\x00\x01\x01')

            log('Setting channel masks...')
            send_command(s, b'\x2F\x08\x01\x00\x20\x00\x00')
            send_command(s, b'\x2F\x08\x00\x00\x00\x00\x00')

            log('Starting network steering...')
            send_command(s, b'\x2F\x05\x02')

            time.sleep(5)

            log('Registering...')
            send_command(s, b'\x24\x00\x01\x04\x01\x00\x01\x00\x00\x02\x00\x00\x06\x00\x02\x00\x00\x06\x00')
        elif sys.argv[2] == 'end':
            log('Setting reset bit and restarting...')
            send_command(s, b'\x21\x09\x03\x00\x00\x01\x03')
            send_command(s, b'\x41\x00\x00')

            time.sleep(3)

            log('Setting role...')
            send_command(s, b'\x21\x09\x87\x00\x00\x01\x02')

            log('Setting channel masks...')
            send_command(s, b'\x2F\x08\x01\x00\x20\x00\x00')
            send_command(s, b'\x2F\x08\x00\x00\x00\x00\x00')

            log('Starting network steering...')
            send_command(s, b'\x2F\x05\x02')

            time.sleep(5)

            log('Registering...')
            send_command(s, b'\x24\x00\x01\x04\x01\x00\x01\x00\x00\x02\x00\x00\x06\x00\x02\x00\x00\x06\x00')
        else:
            raise RuntimeError("Invalid startup sequence.")

    log('Accepting manual commands')

    for line in sys.stdin:
        tokens = line.split()
        tokens = filter(lambda x: len(x) == 2, tokens)
        bs = bytes(map(lambda x: int(x, 16), tokens))

        send_command(s, bs)

    listener.join()
