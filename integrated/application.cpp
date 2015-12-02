#define __PLATFORM_AVR__

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdio.h>

//#include "lcd/avrlcd.h"
//#include "lcd/font.c"
//#include "lcd/ili934x.c"
//#include "lcd/lcd.c"

#include "timer/timer.cpp"
#include "uart/uart.c"

// #include "link/DataLink.h"
// #include "link/rfm12.h"
// #include "link/rfm12.cpp"

//#include "link/timer/timer.cpp"
//#include "link_dummy/link.cpp"
#include "physical/physical.cpp"
#include "link/link.cpp"
#include "network/network.cpp"
#include "transport/transport.cpp"



//C232HM Pinout. Yellow - D1. Orange - D0. Black - Ground.


int main()
{
    _delay_ms(100);  //little delay for the rfm12 to initialize properly
    rfm12_init();    //init the RFM12
    _delay_ms(100);
    sei();
    init_uart0();
    init_timer();
    put_string("\r\n\r\n\r\n\r\nInitialising...");
    
    while(1){
        char dest, source;
        char mode;
        char rmessageflag;

        char temp = '\0'; //temporary character for receiving over uart
        char message[1000] = {0};
        uint16_t i = 0;

        put_string("\r\n\r\nSend or receive ('S' or 'R'): ");
        while(temp != '\r')
        {
            temp = get_char();
            if (temp >= 32 && temp <= 126)
            {
                put_char(temp);
                mode = temp;
            }
            else if ((temp == 8) || (temp == 127)) //Backspace or delete
            {
                put_char(temp);
                mode = 0;
            }
            _delay_ms(1);
        }

        temp = 0;

        if (mode == 'S')
        {
            put_string("\r\n\r\nEnter destination: ");
            while(temp != '\r')
            {
                temp = get_char();
                if (temp >= 32 && temp <= 126)
                {
                    put_char(temp);
                    dest = temp;
                }
                else if ((temp == 8) || (temp == 127)) //Backspace or delete
                {
                    put_char(temp);
                    dest = 0;
                }
                _delay_ms(1);
            }

            temp = 0;

            put_string("\r\n\r\nEnter message: ");

            while(temp != '\r')
            {
                temp = get_char();
                if (temp >= 32 && temp <= 126)
                {
                    put_char(temp);
                    message[i] = temp;
                    i++;
                }
                else if ((temp == 8) || (temp == 127)) //Backspace or delete
                {
                    put_char(temp);
                    i--;
                    message[i] = 0;
                }
                _delay_ms(1);
            }

            put_string("\r\n");
            put_number(strlen(message));
            put_string(" character long message reads: \r\n");
            put_string(message);

            int error = SendData(dest, message);
            if (error) put_string("Error sending message");
        }

        else if (mode == 'R')
        {
            while(1)
            {
                rmessageflag = RecieveData(&source, message);
                if (rmessageflag)
                {
                    put_string("\r\n");
                    put_number(strlen(message));
                    put_string(" character long message from ");
                    put_char(source);
                    put_string(" reads: \r\n");
                    put_string(message);
                    return 0; //THIS NEEDS TO BE DELETED TO RECEIVE MORE THAN 1 MESSAGE
                } 
            }
        }
        //while(1);
       
    }
     return 0;
}