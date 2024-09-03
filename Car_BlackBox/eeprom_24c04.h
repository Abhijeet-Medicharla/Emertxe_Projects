
#ifndef EEPROM_24C04_H
#define	EEPROM_24C04_H

#define WRITE_24C04               0b10100000      //0xA0        
#define READ_24C04                0b10100001      //0xA1       

unsigned char read_eeprom_24c04(unsigned char addr);
void byte_write_eeprom_24c04(unsigned char addr, unsigned char data);
void str_write_eeprom_24c04(unsigned char addr, char *data);

#endif	/* EEPROM_24C04_H */

