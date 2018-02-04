TOOLS = arm-none-eabi

CFLAGS 	= -mcpu=cortex-m3 -mthumb -g
CFLAGS 	+= -I./
CFLAGS 	+= -I./lib/CMSIS/Include
CFLAGS 	+= -I./lib/CMSIS/Device/ST/STM32F1xx/Include
CFLAGS	+= -DSTM32F103x6

AS 		= $(TOOLS)-as
CC 		= $(TOOLS)-g++
LD 		= $(TOOLS)-ld.bfd
OBJCOPY = $(TOOLS)-objcopy
DUMP 	= $(TOOLS)-objdump -d
GDB 	= $(TOOLS)-gdb

OBJS 	= locore.o blink.o USART.o

all: blink.bin blink.dump

blink.dump: blink.elf
	$(DUMP) blink.elf >blink.dump

blink.bin: blink.elf
	$(OBJCOPY) blink.elf blink.bin -O binary

blink.elf: $(OBJS)
	$(LD) -T blink.lds -o blink.elf $(OBJS)

locore.o: locore.s
	$(AS) locore.s -o locore.o

%.o: %.cpp
	$(CC) $(CFLAGS) -c $<

flash: blink.bin
	st-flash --reset write blink.bin 0x08000000

gdb:
	$(GDB) --eval-command="target remote localhost:3333" blink.elf

clean:
	rm -f *.o blink.elf blink.bin blink.dump
