import serial
import time
import datetime
import sys

BEGIN = 0x00
END = 0xFF
THRESHOLD = 2.0
REPORT_INTERVAL = 10

last = None
last_scent = time.time()
correct = b'\x00\x08\xd1\x0d\xb6\x16\xfe\x01\x4b\x12\xfe\x01\x04\x70\x69\x6e\x67\x04\x74\x79\x70\x65\x0f\x66\x61\x6b\x65\x5f\x6c\x69\x67\x68\x74\x5f\x62\x75\x6c\x62\xff'
buf = b''

c_correct = 0
c_incorrect = 0
c_missed = 0

def log(message):
    print('[%s] %s' % (datetime.datetime.now(), message))

def report():
    count = c_correct + c_incorrect + c_missed

    if count % REPORT_INTERVAL == 0:
        log("Stats: count = %d, correct = %.2f, incorrect = %.2f, missed = %.2f" %
            (count, c_correct / count, c_incorrect / count, c_missed / count))

with serial.Serial('/dev/tty.Repleo-PL2303-002012FA', 115200, timeout=0.1) as s:
    while True:
        buf += s.read()

        if len(buf) > 0 and buf[-1] == END:
            if last is not None:
                if buf == correct:
                    c_correct += 1
                    report()
                    log('Received correct message, t = %.3lf' % (time.time() - last))
                else:
                    c_incorrect += 1
                    report()
                    log('Received INCORRECT message, t = %.3lf' % (time.time() - last))
                    log('Message is: ' + ' '.join(map(lambda x: '%02x' % x, buf)))

            last = time.time()
            last_scent = last
            buf = b''
        else:
            if time.time() - last_scent > THRESHOLD:
                c_missed += 1
                report()
                log('MISSED a message')
                last_scent = time.time()
