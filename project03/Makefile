default:
	avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p -c -o project03.o project03.c
	avr-gcc -mmcu=atmega328p -o project03.bin project03.o
	avr-objcopy -O ihex -R eeprom project03.bin project03.hex
	sudo avrdude -F -V -c arduino -p ATMEGA328p -P /dev/ttyACM0 -b 115200 -U flash:w:project03.hex