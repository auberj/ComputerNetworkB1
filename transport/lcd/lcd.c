/*  Author: Steve Gunn
 * Licence: This work is licensed under the Creative Commons Attribution License.
 *           View this license at http://creativecommons.org/about/licenses/
 */

#include <stdio.h>
#include "ili934x.h"
#include "font.h"
#include "lcd.h"

lcd display = {LCDWIDTH, LCDHEIGHT, North, 0, 0, WHITE, BLACK};

void init_lcd()
{
	/* Disable JTAG in software, so that it does not interfere with Port C  */
	/* It will be re-enabled after a power cycle if the JTAGEN fuse is set. */
	MCUCR |= (1<<JTD);
	MCUCR |= (1<<JTD);
	
	/* Configure ports */
	CTRL_DDR = 0x7F;
	DATA_DDR = 0xFF;
	
	init_display_controller();
}

void set_orientation(orientation o)
{
	display.orient = o;
	write_cmd(MEMORY_ACCESS_CONTROL);
	if (o==North) { 
		display.width = LCDWIDTH;
		display.height = LCDHEIGHT;
		write_data(0x48);
	}
	else if (o==West) {
		display.width = LCDHEIGHT;
		display.height = LCDWIDTH;
		write_data(0xE8);
	}
	else if (o==South) {
		display.width = LCDWIDTH;
		display.height = LCDHEIGHT;
		write_data(0x88);
	}
	else if (o==East) {
		display.width = LCDHEIGHT;
		display.height = LCDWIDTH;
		write_data(0x28);
	}
	write_cmd(COLUMN_ADDRESS_SET);
	write_data16(0);
	write_data16(display.width-1);
	write_cmd(PAGE_ADDRESS_SET);
	write_data16(0);
	write_data16(display.height-1);
}

void fill_rectangle(rectangle r, uint16_t col)
{
	uint16_t x, y;
	write_cmd(COLUMN_ADDRESS_SET);
	write_data16(r.left);
	write_data16(r.right);
	write_cmd(PAGE_ADDRESS_SET);
	write_data16(r.top);
	write_data16(r.bottom);
	write_cmd(MEMORY_WRITE);
	for(x=r.left; x<=r.right; x++)
		for(y=r.top; y<=r.bottom; y++)
			write_data16(col);
}

void fill_rectangle_indexed(rectangle r, uint16_t* col)
{
	uint16_t x, y;
	write_cmd(COLUMN_ADDRESS_SET);
	write_data16(r.left);
	write_data16(r.right);
	write_cmd(PAGE_ADDRESS_SET);
	write_data16(r.top);
	write_data16(r.bottom);
	write_cmd(MEMORY_WRITE);
	for(x=r.left; x<=r.right; x++)
		for(y=r.top; y<=r.bottom; y++)
			write_data16(*col++);
}

void clear_screen()
{
	display.x = 0;
	display.y = 0;
	rectangle r = {0, display.width-1, 0, display.height-1};
	fill_rectangle(r, display.background);
}

void display_char(char c)
{
	uint16_t x, y;
	PGM_P fdata; 
	uint8_t bits, mask;
	uint16_t sc=display.x, ec=display.x + 4, sp=display.y, ep=display.y + 7;
	if (c == 10)
	{
		display.x=0;
		display.y+=8; //Added /n to characters
	}
	if (c < 32 || c > 126) return;
	fdata = (PGM_P)((c - ' ')*5 + font5x7);
	write_cmd(PAGE_ADDRESS_SET);
	write_data16(sp);
	write_data16(ep);
	for(x=sc; x<=ec; x++) {
		write_cmd(COLUMN_ADDRESS_SET);
		write_data16(x);
		write_data16(x);
		write_cmd(MEMORY_WRITE);
		bits = pgm_read_byte(fdata++);
		for(y=sp, mask=0x01; y<=ep; y++, mask<<=1)
			write_data16((bits & mask) ? display.foreground : display.background);
	}
	write_cmd(COLUMN_ADDRESS_SET);
	write_data16(x);
	write_data16(x);
	write_cmd(MEMORY_WRITE);
	for(y=sp; y<=ep; y++)
		write_data16(display.background);

	display.x += 6;
	if (display.x > display.width-6) { display.x=0; display.y+=8; }
}

void display_string(char *str)
{
	uint8_t i;
	for(i=0; str[i]; i++) 
		display_char(str[i]);
}

void display_nstring(char *str, uint8_t start, uint8_t n) //Displays n characters in the string from start point called 'start'
{
	uint8_t i;
	for(i=start; i<(start+n); i++) 
		display_char(str[i]);
}

void display_number(uint16_t number) //Displays a number using sprintf. Can display between 0 and 99999
{
	char numberstring[5];
  	snprintf(numberstring, 5, "%d", number); //Convert to string so we can display
  	display_string(numberstring);
}

void display_hex(long hex, uint8_t bytes) //Displys 1 to 4 bytes in hex (if you want up to 8, change 'long'to 'long long')
{
	char hexchars[bytes*2];
	for (int i = 0; i < bytes*2; i++)
	{
		uint8_t temp = 0xF & (hex >> (i*4));
		if (temp > 10)
			hexchars[bytes*2-1-i] = temp + 87;
		else
			hexchars[bytes*2-1-i] = temp + 48;
	}
	display_string("0x");
	display_nstring(hexchars, 0, bytes*2);
}

void display_binary(uint8_t byte) //Displys a byte in binary
{
	char binchars[8];
	for (int i = 7; i > -1; i--)
	{
		if (byte & (1 << i))
			binchars[7-i]='1';
		else
			binchars[7-i]='0'; 
	}
    display_string("0b");
    display_nstring(binchars, 0, 8);
}