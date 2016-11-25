# WinAVR cross-compiler toolchain is used here
CC = avr-gcc
OBJCOPY = avr-objcopy
DUDE = avrdude

CFLAGS = -Wall -Os -Iusbdrv -mmcu=atmega328p -DF_CPU=16000000
OBJFLAGS = -j .text -j .data -O ihex
DUDEFLAGS = -p atmega328p -c arduino -P COM7 -b 57600 -v

# Object files for the firmware
OBJECTS = rotenctest.o encoder.o settings.o

# By default, build the firmware and command-line client, but do not flash
all: rotenctest.hex

# With this, you can flash the firmware by just typing "make flash" on command-line
flash: rotenctest.hex
	$(DUDE) $(DUDEFLAGS) -U flash:w:$<

eeprom: rotenctest.eep
	$(DUDE) $(DUDEFLAGS) -U eeprom:w:$<

# Housekeeping if you want it
clean:
	$(RM) *.o *.hex *.elf

# From .elf file to .hex
%.hex: %.elf
	$(OBJCOPY) $(OBJFLAGS) $< $@

# rotenctest.elf requires additional objects to the firmware, not just rotenctest.o
rotenctest.elf: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

# From C source to .o object file
%.o: %.c	
	$(CC) $(CFLAGS) -c $< -o $@