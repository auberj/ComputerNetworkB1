//Author: dm6g13

#include "uart1.h"
#include <stdio.h>

void init_uart1(void) //Function copied from Steve Gunn's library
{
	/* Configure 9600 baud, 8-bit, no parity and one stop bit */
	const int baud_rate = 9600;
	UBRR1H = (F_CPU/(baud_rate*16L)-1) >> 8;
	UBRR1L = (F_CPU/(baud_rate*16L)-1);
	UCSR1B = _BV(RXEN1) | _BV(TXEN1);
	UCSR1C = _BV(UCSZ10) | _BV(UCSZ11);
}

char get_1_char(void) //Function copied from Steve Gunn's library
{
	while(!(UCSR1A & _BV(RXC1)));
	return UDR1;
}

void put_1_char(char ch) //Function copied from Steve Gunn's library
{
	while (!(UCSR1A & _BV(UDRE1)));
	UDR1 = ch;
}

void put_1_string(char *str) //Function copied from Steve Gunn's library
{
	int i;
	for(i=0; str[i]; i++) put_char(str[i]);
}