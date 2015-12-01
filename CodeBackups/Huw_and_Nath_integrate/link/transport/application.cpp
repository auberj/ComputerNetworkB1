#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

//#include "lcd/avrlcd.h"
//#include "lcd/font.c"
//#include "lcd/ili934x.c"
//#include "lcd/lcd.c"

#include "timer/timer.cpp"
#include "uart/uart.c"

#include "network/network.cpp"
#include "transport/transport.cpp"

//C232HM Pinout. Yellow - D1. Green - D0. Black - Ground.

int main()
{
    init_uart0();
    init_timer();
    put_string("\r\n\r\n\r\n\r\nInitialising...\r\n\r\n");
    char dest = 'D';
    char message[1000] = "Hello World!";
    //Above message is 107 characters
    put_number(strlen(message));
    put_string(" character long message reads:\r\n");
    put_string(message);

    int error = SendData(dest, message);
    if (error) put_string("Error sending message");

    while(1);
    return 0;
}