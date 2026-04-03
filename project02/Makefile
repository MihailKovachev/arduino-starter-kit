default:
	avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p -c -o project02.o project02.c
	avr-gcc -o project02.bin project02.o
	avr-objcopy -O ihex -R eeprom project02.bin project02.hex
	sudo avrdude -F -V -c arduino -p ATMEGA328p -P /dev/ttyACM0 -b 115200 -U flash:w:project02.hex