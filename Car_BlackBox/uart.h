
#ifndef UART_H
#define	UART_H

#define FOSC                20000000

void init_uart(unsigned long baud);
void uart_send_char(unsigned char data);
void uart_send_str(const char *s);

#endif	/* UART_H */