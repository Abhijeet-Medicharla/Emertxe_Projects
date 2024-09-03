#include "main.h"

unsigned char read_eeprom_24c04(unsigned char addr)
{
    unsigned char data;
    
    i2c_start();
    i2c_write(WRITE_24C04);
    i2c_write(addr);
    i2c_rep_start();
    i2c_write(READ_24C04);
    data = i2c_read(0);
    i2c_stop();
    
    return data;
}

void byte_write_eeprom_24c04(unsigned char addr, unsigned char data)
{
    i2c_start();
    i2c_write(WRITE_24C04);
    i2c_write(addr);
    i2c_write(data);
    i2c_stop();
}

void str_write_eeprom_24c04(unsigned char addr, char *data)
{
    while(*data != 0)
    {
        byte_write_eeprom_24c04(addr, *data);
        data++;
        addr++;
    }
}

