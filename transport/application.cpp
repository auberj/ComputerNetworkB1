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
    display_string("Initialising...\n\n");
    char dest = 'D';
    char message[1000] = "Venison. Tongue alcatra rump chicken pork belly. Turducken bresaola jerky drumstick short loin pork chop beef ribs ball tip turkey";

    display_number(strlen(message));
    display_string(" character long message reads:\n");
    display_string(message);
    display_string("\n\n");

    int error = SendData(dest, message);
    if (error) display_string("Error sending message");

    while(1)
    {
    	_delay_ms(1000);
    	display_string(".");
    }
    return 0;
}