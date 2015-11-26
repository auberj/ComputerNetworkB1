#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>

//#include "lcd/avrlcd.h"
//#include "lcd/font.c"
//#include "lcd/ili934x.c"
//#include "lcd/lcd.c"

#include "timer/timer.cpp"
#include "uart/uart.c"

#include "link_dummy/link.cpp"
#include "network/network.cpp"
#include "transport/transport.cpp"

//C232HM Pinout. Yellow - D1. Orange - D0. Black - Ground.


int main()
{
    while(1){
        init_uart0();
        init_timer();
        put_string("\r\n\r\n\r\n\r\nInitialising...\r\n\r\n");
        char dest = 'N';
        char temp = '\0';
        char message[1000] = {0};
        uint16_t i = 0;

        put_string("Enter message: ");

        while(temp != '\r')
        {
            temp = get_char();
            if (temp >= 32 && temp <= 126)
            {
                put_char(temp);
                message[i] = temp;
                i++;
            }
            _delay_ms(1);
        }

        //get message over UART...
        //Above message is 107 characters
        put_string("\r\n");
        put_number(strlen(message));
        put_string(" character long message reads:\r\n");
        put_string(message);

        int error = SendData(dest, message);
        if (error) put_string("Error sending message");

        //while(1);
       
    }
     return 0;
}