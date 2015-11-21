#include "lcd/avrlcd.h"
#include "lcd/font.c"
#include "lcd/ili934x.c"
#include "lcd/lcd.c"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "network/network.cpp"
#include "transport/transport.cpp"

int main()
{
    init_lcd();
    set_orientation(East);
    display_string("Initialising...\n");
    display_hex(255);
    display_number(255);
    display_string("\n");
    char dest = 'D';
    char message[] = "Bacon short loin, kielbasa porchetta pancetta tongue tail venison. Tongue alcatra rump chicken pork belly. Turducken bresaola jerky drumstick short loin pork chop beef ribs ball tip turkey.";
    int error = SendData(dest, message);
    while(1)
    {
    	_delay_ms(1000);
    	display_string(".");
    }
    return 0;
}