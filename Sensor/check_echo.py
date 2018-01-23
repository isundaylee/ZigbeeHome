import serial
import time
import datetime
import sys

with serial.Serial('/dev/tty.Repleo-PL2303-002012FA', 115200, timeout=1) as s:
    while True:
        correct = b''
        for i in range(50):
            s.write(bytes([i]))
            correct += bytes([i])

        time.sleep(0.1)
        buf = s.read(50)

        if buf == correct:
            sys.stdout.write('.')
            sys.stdout.flush()
        else:
            print('\nFAILURE %d!' % len(buf))
