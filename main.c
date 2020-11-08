#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <string.h>
#include <util/delay.h>

#include "adc.h"
#include "gpio.h"
#include "i2c.h"
#include "serial.h"
#include "timer.h"

char myName[8];

void main (void) {

	i2c_init();
	uart_init();

	sei();

	eeprom_write_byte(WRITE_ADDRESS, 'E');
	eeprom_write_byte(0x11, 'n');
	eeprom_write_byte(0x12, 'o');
	eeprom_write_byte(0x13, 'c');

	myName[0] = eeprom_read_byte(0x10);
	myName[1] = eeprom_read_byte(0x11);
	myName[2] = eeprom_read_byte(0x12);
	myName[3] = eeprom_read_byte(0x13);


	printf_P(PSTR("DATA FROM EEPROM: %s \n"), myName);
	
	//for (int i = 0; i < strlen(myName); i++) {
	//	printf("%x", myName[i]);
	//}


	while (1) {

	}
}

