import serial
import time
import datetime
import sys

BEGIN = 0x00;
END = 0xFF;
ESC = 0xFE;
ESC_BEGIN = 0x01;
ESC_END = 0x02;
ESC_ESC = 0x03;

DEVICE_TYPE_MAP = {
    0x01: 'smoke_detector'
}

last = None
buf = b''

def log(message):
    print('[%s] %s' % (datetime.datetime.now(), message))
    sys.stdout.flush()

def unescape(buf):
    result = b''

    if buf[0] != 0x00:
        log('Ignored packet with wrong start byte %02x, length %d' % (buf[0], len(buf)))
        return None
    assert(buf[-1] == 0xFF);

    i = 1
    while i < len(buf) - 1:
        if buf[i] == ESC:
            if buf[i + 1] == ESC_BEGIN:
                result = result + bytes([BEGIN])
            elif buf[i + 1] == ESC_END:
                result = result + bytes([END])
            elif buf[i + 1] == ESC_ESC:
                result = result + bytes([ESC])
            else:
                assert(False)
            i += 2
        else:
            result = result + buf[i:i+1]
            i += 1

    return result

def mac_to_string(mac):
    return ':'.join(map(lambda x: '%02x' % x, mac))

with serial.Serial('/dev/tty.Repleo-PL2303-002012FA', 115200, timeout=0.1) as s:
    while True:
        buf += s.read()

        if len(buf) > 0 and buf[-1] == END:
            buf = unescape(buf)
            if buf is None:
                buf = b''
                continue

            log_string = 'From %s (%s):' % (DEVICE_TYPE_MAP[buf[8]], mac_to_string(buf[:8]))

            start = 9
            while start != len(buf):
                if buf[start] == 0x01:
                    if buf[start + 1] == 1:
                        log_string += ' true'
                    else:
                        log_string += ' false'
                    start += 2
                elif buf[start] == 0x02:
                    log_string += ' %s' % buf[start + 2: start + 2 + buf[start + 1]].decode()
                    start += (buf[start + 1] + 2)
                elif buf[start] == 0x05:
                    log_string += ' %d' % (buf[start + 1] * 256 + buf[start + 2])
                    start += 3
                else:
                    assert(False)

            log(log_string)

            buf = b''
