#include <inttypes.h>
#include <avr/io.h>

void init_uart0(void);

char get_char(void);

void put_char(char ch);

void put_string(char *str);

void put_nstring(char *str, uint8_t start, uint8_t n); //Displays n characters in the string from start point called 'start'

void put_number(uint16_t number); //Displays a number using sprintf. Can display between 0 and 99999999

void put_hex(unsigned long hex, uint8_t bytes); //Displys 1 to 4 bytes in hex (if you want up to 8, change 'long'to 'long long')

void put_binary(uint8_t byte); //Displys a byte in binary