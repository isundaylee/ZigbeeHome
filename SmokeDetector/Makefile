GCC_PREFIX  = arm-none-eabi
LLVM_PREFIX = /usr/local/opt/llvm/bin

CC 			= $(LLVM_PREFIX)/clang++
LD 			= $(GCC_PREFIX)-ld.bfd
OBJCOPY 	= $(GCC_PREFIX)-objcopy
DUMP 		= $(GCC_PREFIX)-objdump -d
GDB 		= $(GCC_PREFIX)-gdb
SIZE 		= $(GCC_PREFIX)-size

CC_FLAGS 	= -target arm-none-eabi
CC_FLAGS	+= -mcpu=cortex-m0
CC_FLAGS	+= -mthumb
CC_FLAGS	+= -mfloat-abi=soft

CC_FLAGS	+= -g
CC_FLAGS	+= -O2
CC_FLAGS    += -std=c++17
CC_FLAGS 	+= -nostdlib
CC_FLAGS    += -ffreestanding
CC_FLAGS    += -fno-exceptions

CC_FLAGS 	+= -Wall
CC_FLAGS 	+= -Wextra
CC_FLAGS 	+= -Wold-style-cast
CC_FLAGS 	+= -Werror

CC_FLAGS 	+= -I./include
CC_FLAGS 	+= -isystem ./lib/CMSIS/Include
CC_FLAGS 	+= -isystem ./lib/CMSIS/Device/ST/STM32F1xx/Include
CC_FLAGS 	+= -isystem ./lib/CMSIS/Device/ST/STM32L0xx/Include
CC_FLAGS	+= -DSTM32L011xx

CC_FLAGS_AS	= -target arm-none-eabi
CC_FLAGS_AS	+= -mcpu=cortex-m0
CC_FLAGS_AS	+= -mfloat-abi=soft

LD_FLAGS	= -Llib/libaeabi-cortexm0
LD_FLAGS	+= -laeabi-cortexm0
CC_FLAGS	+= -mcpu=cortex-m0

BUILDDIR 	= build
SRCDIR		= src

NAME		= SmokeDetector

HEADERS		= $(wildcard include/*.h)
CPPOBJS 	= $(patsubst $(SRCDIR)/%.cpp, $(BUILDDIR)/%.o, $(wildcard $(SRCDIR)/*.cpp))
OBJS 		= $(CPPOBJS) $(BUILDDIR)/Boot.o $(BUILDDIR)/$(NAME).o

AEABI_LIB	= lib/libaeabi-cortexm0/libaeabi-cortexm0.a

all: $(BUILDDIR)/$(NAME).bin

################################################################################
# Compilation stage
################################################################################

$(AEABI_LIB):
	cd lib/libaeabi-cortexm0 && make

$(CPPOBJS): $(BUILDDIR)/%.o: $(SRCDIR)/%.cpp $(HEADERS)
	$(CC) -c $(CC_FLAGS) $< -o $@

$(BUILDDIR)/$(NAME).o: apps/$(NAME).cpp $(HEADERS)
	$(CC) -c $(CC_FLAGS) $< -o $@

$(BUILDDIR)/Boot.o: $(SRCDIR)/Boot.s
	$(CC) -c $(CC_FLAGS_AS) $< -o $@

################################################################################
# Linking stage
################################################################################

$(BUILDDIR)/$(NAME).elf: $(OBJS) $(SRCDIR)/LinkScript.lds $(AEABI_LIB)
	$(LD) -T $(SRCDIR)/LinkScript.lds $(OBJS) $(LD_FLAGS) -o $@

$(BUILDDIR)/$(NAME).dump: $(BUILDDIR)/$(NAME).elf
	$(DUMP) $< >$@

$(BUILDDIR)/$(NAME).bin: $(BUILDDIR)/$(NAME).elf size
	$(OBJCOPY) $< $@ -O binary

################################################################################
# Utility functions
################################################################################

flash: $(BUILDDIR)/$(NAME).bin
	st-flash --reset write $(BUILDDIR)/$(NAME).bin 0x08000000

gdb: $(BUILDDIR)/$(NAME).elf
	$(GDB) $(BUILDDIR)/$(NAME).elf

clean:
	rm -f $(BUILDDIR)/*
	rm -f $(AEABI_LIB)

size: $(BUILDDIR)/$(NAME).elf
	$(SIZE) $(BUILDDIR)/$(NAME).elf
