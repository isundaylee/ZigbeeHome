# Arduino Makefile. Refer to https://github.com/sudar/Arduino-Makefile

ARDUINO_DIR         = /Applications/Arduino.app/Contents/Java
ARDMK_DIR           = /usr/local/opt/arduino-mk
AVR_TOOLS_DIR       = /Applications/Arduino.app/Contents/Java/hardware/tools/avr/

MONITOR_CMD         = cu
MONITOR_PORT        = /dev/cu.usbserial
MONITOR_BAUDRATE    = 9600

ISP_PROG            = usbasp
ISP_PORT			= usb
ISP_LOW_FUSE		= 0xE2
# ISP_PORT            = /dev/cu.wchusbserialfa1220
# ISP_PORT            = /dev/cu.usbserial-A9M9DV3R

ALTERNATE_CORE 	    = ATTinyCore
ALTERNATE_CORE_PATH = /Users/jiahao/Library/Arduino15/packages/ATTinyCore/hardware/avr/1.1.4
BOARD_TAG 		    = attinyx5
BOARD_SUB 		    = 85
VARIANT 		    = tinyX5
F_CPU 			    = 8000000L

include /usr/local/opt/arduino-mk/Arduino.mk

u: ispload

p:
	python3 check_pings.py
