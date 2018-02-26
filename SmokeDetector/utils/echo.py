import serial
import time
import numpy
import threading
import datetime
import sys

if sys.argv[1] == 'slave':
    DEV = '/dev/cu.wchusbserial401140'
elif sys.argv[1] == 'master':
    DEV = '/dev/cu.usbserial-A9M9DV3R'
else:
    raise RuntimeError("Invalid device given.")

with serial.Serial(DEV, 115200) as s:
    time.sleep(0.500)

    while True:
        sys.stdout.write(s.read().decode('ascii'))
        sys.stdout.flush()
