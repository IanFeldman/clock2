# Target MCU
MCU = cortex-m0
CHIP = LPC812

# Toolchain
CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy
CFLAGS = -mcpu=$(MCU) -mthumb -O0 -g -Wall -std=c11 -I./cmsis -ffreestanding -fno-builtin
LDFLAGS = -T linker.ld -nostdlib -lgcc

# Files
OBJS = startup_lpc812.o main.o i2c.o uart.o util.o input.o display.o
TARGET = clock

all: $(TARGET).hex

$(TARGET).elf: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex $< $@

clean:
	rm -f *.o *.elf *.hex

flash: $(TARGET).hex
ifndef PORT
	$(error PORT is not defined for flash)
endif
	lpc21isp -verify -wipe $< ${PORT} 115200 12000

