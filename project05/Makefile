default:
	avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p -c -o project05.o project05.c
	avr-gcc -mmcu=atmega328p -o project05.bin project05.o
	avr-objcopy -O ihex -R eeprom project05.bin project05.hex
	sudo avrdude -F -V -c arduino -p ATMEGA328p -P /dev/ttyACM0 -b 115200 -U flash:w:project05.hex