import serial
import time
import numpy

def avg(readings):
    return sum(readings) / len(readings)

def std(readings):
    return numpy.std(readings)

def rg(readings):
    return max(readings) - min(readings)

with serial.Serial('/dev/cu.wchusbserial401220', 115200) as s:
    time.sleep(1)

    readings = []

    while True:
        start = time.time()
        s.write(b'\xFE\x00\x21\x01\x20')
        s.read(7)
        end = time.time()

        readings.append(1000 * (end - start))
        print("last = %.1f ms, avg = %.1f ms, std = %.1f ms, range = %.1f ms, min = %.1f ms, max = %.1f ms" %
            (readings[-1], avg(readings), std(readings), rg(readings), min(readings), max(readings)))
