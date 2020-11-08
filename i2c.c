#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#include <stdio.h>
#include <string.h>

#include "i2c.h"

void i2c_init(void) {
	//100 000 = 16 000 000 / (16 + (2 * TWBR * 1)

	TWCR = (1 << TWEN); //This is to enable 2wire
	TWBR = 72; // this makes the clock speed 100khz
	TWSR = 0;
}

void i2c_meaningful_status(uint8_t status) {
	switch (status) {
		case 0x08: // START transmitted, proceed to load SLA+W/R
			printf_P(PSTR("START\n"));
			break;
		case 0x10: // repeated START transmitted, proceed to load SLA+W/R
			printf_P(PSTR("RESTART\n"));
			break;
		case 0x38: // NAK or DATA ARBITRATION LOST
			printf_P(PSTR("NOARB/NAK\n"));
			break;
		// MASTER TRANSMIT
		case 0x18: // SLA+W transmitted, ACK received
			printf_P(PSTR("MT SLA+W, ACK\n"));
			break;
		case 0x20: // SLA+W transmitted, NAK received
			printf_P(PSTR("MT SLA+W, NAK\n"));
				break;
		case 0x28: // DATA transmitted, ACK received
			printf_P(PSTR("MT DATA+W, ACK\n"));
			break;
		case 0x30: // DATA transmitted, NAK received
			printf_P(PSTR("MT DATA+W, NAK\n"));
			break;
		// MASTER RECEIVE
		case 0x40: // SLA+R transmitted, ACK received
			printf_P(PSTR("MR SLA+R, ACK\n"));
			break;
		case 0x48: // SLA+R transmitted, NAK received
			printf_P(PSTR("MR SLA+R, NAK\n"));
			break;
		case 0x50: // DATA received, ACK sent
			printf_P(PSTR("MR DATA+R, ACK\n"));
			break;
		case 0x58: // DATA received, NAK sent
			printf_P(PSTR("MR DATA+R, NAK\n"));
			break;
		default:
			printf_P(PSTR("N/A %02X\n"), status);
			break;
	}
}

inline void i2c_start() {
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT))); // Wait till start condition is transmitted
}

inline void i2c_stop() {
	// Clear TWI interrupt flag, Put stop condition on SDA, Enable TWI
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
	while (!(TWCR & (1 << TWSTO)));  // Wait till stop condition is transmitted
}

inline uint8_t i2c_get_status(void) {
	uint8_t i2cStatus;

	i2cStatus = (TWSR & 0xF8); // 0xF8 equal 1111 1000 the last arent used to check the status. 
							   //The last 2 bits can be read and written, and control the bit rate prescaler.

	return i2cStatus;
}

inline void i2c_xmit_addr(uint8_t address, uint8_t rw) {
	
	TWDR = (address & 0xfe) | (rw & 0x01); // Först är det addressen man vill skriva på o sedan är det R/W biten
	
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
}

inline void i2c_xmit_byte(uint8_t data) {
	TWDR = data; // it writes data byte to TWDR

	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
}

inline uint8_t i2c_read_ACK() {

	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);

	while (!(TWCR & (1 << TWINT)));

	return TWDR;

}

inline uint8_t i2c_read_NAK() {
	TWCR = (1 << TWINT) | (1 << TWEN);

	while (!(TWCR & (1 << TWINT)));

	return TWDR;
}

inline void eeprom_wait_until_write_complete() {
	while (i2c_get_status() != 0x18) { // This does a loops untils get status gets to 0x18 and then starts the rest of the function.

		i2c_start();

		i2c_xmit_addr(EEPROM_ADDR, I2C_W);
	}
}
	uint8_t eeprom_read_byte(uint8_t addr) {
		uint8_t data;

		i2c_start();
		i2c_xmit_addr(EEPROM_ADDR, I2C_W);
		i2c_meaningful_status(i2c_get_status());
		i2c_xmit_byte(addr);
		i2c_meaningful_status(i2c_get_status());
		i2c_start();
		i2c_xmit_addr(EEPROM_ADDR, I2C_R);
		i2c_meaningful_status(i2c_get_status());

		data = i2c_read_NAK();
		return data;
	}

	void eeprom_write_byte(uint8_t addr, uint8_t data) {
		i2c_start();

		i2c_xmit_addr(EEPROM_ADDR, I2C_W);
		i2c_meaningful_status(i2c_get_status());

		i2c_xmit_byte(addr);
		i2c_meaningful_status(i2c_get_status());

		i2c_xmit_byte(data);
		i2c_meaningful_status(i2c_get_status());

		i2c_stop();

		eeprom_wait_until_write_complete();

	}





//
//void eeprom_write_page(uint8_t addr, uint8_t *data) {
//	// ... (VG)
//}
//
//void eeprom_sequential_read(uint8_t *buf, uint8_t start_addr, uint8_t len) {
//	// ... (VG)
//}
