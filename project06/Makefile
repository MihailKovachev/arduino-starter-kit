default:
	avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p -c -o project06.o project06.c
	avr-gcc -mmcu=atmega328p -o project06.bin project06.o
	avr-objcopy -O ihex -R eeprom project06.bin project06.hex
	sudo avrdude -F -V -c arduino -p ATMEGA328p -P /dev/ttyACM0 -b 115200 -U flash:w:project06.hex