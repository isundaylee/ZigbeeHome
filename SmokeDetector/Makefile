TOOLS = arm-none-eabi

AS 			= $(TOOLS)-as
CC 			= $(TOOLS)-g++
LD 			= $(TOOLS)-ld.bfd
OBJCOPY 	= $(TOOLS)-objcopy
DUMP 		= $(TOOLS)-objdump -d
GDB 		= $(TOOLS)-gdb

CFLAGS 		= -mcpu=cortex-m3 -mthumb -g
CFLAGS 		+= -I./include
CFLAGS 		+= -I./lib/CMSIS/Include
CFLAGS 		+= -I./lib/CMSIS/Device/ST/STM32F1xx/Include
CFLAGS		+= -DSTM32F103x6

BUILDDIR 	= build
SRCDIR		= src

CPPOBJS 	= $(BUILDDIR)/blink.o $(BUILDDIR)/USART.o
OBJS 		= $(CPPOBJS) $(BUILDDIR)/locore.o

all: $(BUILDDIR)/blink.bin

################################################################################
# Compilation stage
################################################################################

$(CPPOBJS): $(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/locore.o: $(SRCDIR)/locore.s
	$(AS) $< -o $@

################################################################################
# Linking stage
################################################################################

$(BUILDDIR)/blink.elf: $(OBJS) $(SRCDIR)/blink.lds
	$(LD) -T $(SRCDIR)/blink.lds $(OBJS) -o $@

$(BUILDDIR)/blink.dump: $(BUILDDIR)/blink.elf
	$(DUMP) $< >$@

$(BUILDDIR)/blink.bin: $(BUILDDIR)/blink.elf
	$(OBJCOPY) $< $@ -O binary

################################################################################
# Utility functions
################################################################################

flash: $(BUILDDIR)/blink.bin
	st-flash --reset write $(BUILDDIR)/blink.bin 0x08000000

gdb:
	$(GDB) --eval-command="target remote localhost:3333" blink.elf

clean:
	rm -f $(BUILDDIR)/*
