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

NAME		= SmokeDetector

CPPOBJS 	= $(BUILDDIR)/$(NAME).o $(BUILDDIR)/USART.o $(BUILDDIR)/SPI.o
OBJS 		= $(CPPOBJS) $(BUILDDIR)/Boot.o

all: $(BUILDDIR)/$(NAME).bin

################################################################################
# Compilation stage
################################################################################

$(CPPOBJS): $(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/Boot.o: $(SRCDIR)/Boot.s
	$(AS) $< -o $@

################################################################################
# Linking stage
################################################################################

$(BUILDDIR)/$(NAME).elf: $(OBJS) $(SRCDIR)/$(NAME).lds
	$(LD) -T $(SRCDIR)/$(NAME).lds $(OBJS) -o $@

$(BUILDDIR)/$(NAME).dump: $(BUILDDIR)/$(NAME).elf
	$(DUMP) $< >$@

$(BUILDDIR)/$(NAME).bin: $(BUILDDIR)/$(NAME).elf
	$(OBJCOPY) $< $@ -O binary

################################################################################
# Utility functions
################################################################################

flash: $(BUILDDIR)/$(NAME).bin
	st-flash --reset write $(BUILDDIR)/$(NAME).bin 0x08000000

gdb:
	$(GDB) --eval-command="target remote localhost:3333" $(NAME).elf

clean:
	rm -f $(BUILDDIR)/*
