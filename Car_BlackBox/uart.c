#include "main.h"

void init_uart(unsigned long baud)
{
    /* Setting RC6 and RC7 to work as Serial Port */
    SPEN = 1;
    
    /* Continuous Reception Enable Bit */
    CREN = 1;
    
    /* Baud Rate Setting Register */
    SPBRG = (unsigned char)((FOSC / (16 * baud)) - 1);
    
    /* Receive Interruot Enable Bit (Enabling the serial port Interrupt) */
    RCIE = 1;
}

void uart_send_char(unsigned char data)
{
    /* Transmit the data to the Serial Port */
    TXREG = data;
    
    /* Wait till the transmission is complete */
    do
    {
        continue;
    } while (TXIF != 1);
    
    /* Clear the interrupt flag */
    TXIF = 0;
}

void uart_send_str(const char *s)
{
    while (*s)
    {
        uart_send_char(*s++);
    }
}