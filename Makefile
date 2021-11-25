MCU = atmega328p
CRYSTAL = 16000000

SERIAL_BAUDRATE=57600

CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE = avr-size

CFLAGS = -g -mmcu=$(MCU) -Wall -Os -fno-inline-small-functions -fno-split-wide-types -D F_CPU=$(CRYSTAL) -D USART_BAUD=$(SERIAL_BAUDRATE)

all:
	$(CC) $(CFLAGS) -c main.c
	$(CC) $(CFLAGS) -c nokia5110.c
	$(CC) $(CFLAGS) main.o nokia5110.o -o code.elf
	$(OBJCOPY) -R .eeprom -O ihex code.elf code.hex
	$(OBJDUMP) -d code.elf > code.lst
	$(OBJDUMP) -h code.elf > code.sec
	$(SIZE) code.elf

serial_sim:
	socat -d -d  pty,link=/tmp/ttyS10,raw,echo=0 pty,link=/tmp/ttyS11,raw,echo=0

clean:
	rm -f *.o *.map *.elf *.sec *.lst *.hex *~
