GCC_PREFIX  = arm-none-eabi
LLVM_PREFIX = /usr/local/opt/llvm/bin

CC 			= $(LLVM_PREFIX)/clang++
LD 			= $(GCC_PREFIX)-ld.bfd
OBJCOPY 	= $(GCC_PREFIX)-objcopy
DUMP 		= $(GCC_PREFIX)-objdump -d
GDB 		= $(GCC_PREFIX)-gdb

CC_FLAGS 	= -target arm-none-eabi
CC_FLAGS	+= -mcpu=cortex-m0
CC_FLAGS	+= -mthumb
CC_FLAGS	+= -mfloat-abi=soft
CC_FLAGS	+= -g
CC_FLAGS 	+= -nostdlib
CC_FLAGS 	+= -I./include
CC_FLAGS 	+= -I./lib/CMSIS/Include
CC_FLAGS 	+= -I./lib/CMSIS/Device/ST/STM32F1xx/Include
CC_FLAGS 	+= -I./lib/CMSIS/Device/ST/STM32L0xx/Include
CC_FLAGS	+= -DSTM32L011xx

CC_FLAGS_AS	= -target arm-none-eabi
CC_FLAGS_AS	+= -mcpu=cortex-m0
CC_FLAGS_AS	+= -mfloat-abi=soft

BUILDDIR 	= build
SRCDIR		= src

NAME		= SmokeDetector

CPPOBJS 	= $(BUILDDIR)/$(NAME).o $(BUILDDIR)/USART.o
OBJS 		= $(CPPOBJS) $(BUILDDIR)/Boot.o

all: $(BUILDDIR)/$(NAME).bin

################################################################################
# Compilation stage
################################################################################

$(CPPOBJS): $(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) -c $(CC_FLAGS) $< -o $@

$(BUILDDIR)/Boot.o: $(SRCDIR)/Boot.s
	$(CC) -c $(CC_FLAGS_AS) $< -o $@

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
	st-flash write $(BUILDDIR)/$(NAME).bin 0x08000000

gdb: $(BUILDDIR)/$(NAME).elf
	$(GDB) --eval-command="target remote localhost:3333" $(BUILDDIR)/$(NAME).elf

clean:
	rm -f $(BUILDDIR)/*
