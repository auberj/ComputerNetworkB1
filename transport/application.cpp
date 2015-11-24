#include "lcd/avrlcd.h"
#include "lcd/font.c"
#include "lcd/ili934x.c"
#include "lcd/lcd.c"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "timer/timer.cpp"
#include "uart/uart.c"

#include "network/network.cpp"
#include "transport/transport.cpp"

int main()
{
    init_lcd();
    set_orientation(East);
    init_uart0();
    init_timer();
    display_number(millis());
    display_string("Initialising...\n\n");
    char dest = 'D';
    char message[1000] = "Hello World! This is a test message written by Dominic Maskell as part of the Computer Networks coursework.";

    display_number(strlen(message));
    display_string(" character long message reads:\n");
    display_string(message);
    display_string("\n\n");

    int error = SendData(dest, message);
    if (error) display_string("Error sending message");

    while(1);
    return 0;
}