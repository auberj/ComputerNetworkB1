//Author: dm6g13

#include "uart.h"
#include <stdio.h>

void init_uart0(void) //Function copied from Steve Gunn's library
{
	/* Configure 9600 baud, 8-bit, no parity and one stop bit */
	const int baud_rate = 9600;
	UBRR0H = (F_CPU/(baud_rate*16L)-1) >> 8;
	UBRR0L = (F_CPU/(baud_rate*16L)-1);
	UCSR0B = _BV(RXEN0) | _BV(TXEN0);
	UCSR0C = _BV(UCSZ00) | _BV(UCSZ01);
}

char get_char(void) //Function copied from Steve Gunn's library
{
	while(!(UCSR0A & _BV(RXC0)));
	return UDR0;
}

void put_char(char ch) //Function copied from Steve Gunn's library
{
	while (!(UCSR0A & _BV(UDRE0)));
	UDR0 = ch;
}

void put_string(char *str) //Function copied from Steve Gunn's library
{
	int i;
	for(i=0; str[i]; i++) put_char(str[i]);
}

void put_nstring(char *str, uint8_t start, uint8_t n) //Displays n characters in the string from start point called 'start'
{
	uint8_t i;
	for(i=start; i<(start+n); i++) 
		put_char(str[i]);
}

void put_number(uint16_t number) //Displays a number using sprintf. Can display between 0 and 99999999
{
	char numberstring[8];
  	snprintf(numberstring, 8, "%d", number); //Convert to string so we can display
  	put_string(numberstring);
}

void put_hex(unsigned long hex, uint8_t bytes) //Displys 1 to 4 bytes in hex (if you want up to 8, change 'long'to 'long long')
{
	char hexchars[bytes*2];
	for (int i = 0; i < bytes*2; i++)
	{
		uint8_t temp = 0xF & (hex >> (i*4));
		if (temp >= 10) //A to F
			hexchars[bytes*2-1-i] = temp + 87;
		else
			hexchars[bytes*2-1-i] = temp + 48;
	}
	put_string("0x");
	put_nstring(hexchars, 0, bytes*2);
}

void put_binary(uint8_t byte) //Displys a byte in binary
{
	char binchars[8];
	for (int i = 7; i >= 0; i--)
	{
		if (byte & (1 << i))
			binchars[7-i]='1';
		else
			binchars[7-i]='0'; 
	}
    put_string("0b");
    put_nstring(binchars, 0, 8);
}

//debugging uart


char get_debug_char(void) //Function copied from Steve Gunn's library
{
	while(!(UCSR0A & _BV(RXC0)));
	return UDR0;
}

void put_debug_char(char ch) //Function copied from Steve Gunn's library
{
	#ifdef debug_on
	while (!(UCSR0A & _BV(UDRE0)));
	UDR0 = ch;
	#endif
}

void put_debug_string(char *str) //Function copied from Steve Gunn's library
{
	int i;
	for(i=0; str[i]; i++) put_debug_char(str[i]);
}

void put_debug_nstring(char *str, uint8_t start, uint8_t n) //Displays n characters in the string from start point called 'start'
{
	uint8_t i;
	for(i=start; i<(start+n); i++) 
		put_debug_char(str[i]);
}

void put_debug_number(uint16_t number) //Displays a number using sprintf. Can display between 0 and 99999999
{
	char numberstring[8];
  	snprintf(numberstring, 8, "%d", number); //Convert to string so we can display
  	put_debug_string(numberstring);
}

void put_debug_hex(unsigned long hex, uint8_t bytes) //Displys 1 to 4 bytes in hex (if you want up to 8, change 'long'to 'long long')
{
	char hexchars[bytes*2];
	for (int i = 0; i < bytes*2; i++)
	{
		uint8_t temp = 0xF & (hex >> (i*4));
		if (temp >= 10) //A to F
			hexchars[bytes*2-1-i] = temp + 87;
		else
			hexchars[bytes*2-1-i] = temp + 48;
	}
	put_debug_string("0x");
	put_debug_nstring(hexchars, 0, bytes*2);
}

void put_debug_binary(uint8_t byte) //Displys a byte in binary
{
	char binchars[8];
	for (int i = 7; i >= 0; i--)
	{
		if (byte & (1 << i))
			binchars[7-i]='1';
		else
			binchars[7-i]='0'; 
	}
    put_debug_string("0b");
    put_debug_nstring(binchars, 0, 8);
}