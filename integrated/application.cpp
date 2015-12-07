#define __PLATFORM_AVR__

#define callsign 'N' //Change this! put to A when syncing

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdio.h>

#include "timer/timer.cpp"
#include "uart/uart.c"

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
        char encryption;

        char temp = '\0'; //temporary character for receiving over uart
        char message[200] = {0};
        char sessionkey[20] = {0};
        uint16_t i = 0;

        put_string("\r\nEncrypt session ('Y' or 'N'): ");
        while(temp != '\r')
        {
            temp = get_char();
            if (temp >= 32 && temp <= 126)
            {
                put_char(temp);
                encryption = temp;
            }
            else if ((temp == 8) || (temp == 127)) //Backspace or delete
            {
                put_char(temp);
                encryption = 0;
            }
            _delay_ms(1);
        }

        if (encryption == 'Y') encryption = 1;
        if (encryption == 'N') encryption = 0;

        temp = 0;

        if (encryption)
        {
            put_string("\r\nEnter session key: ");
            while(temp != '\r')
            {
                temp = get_char();
                if (temp >= 32 && temp <= 126)
                {
                    put_char(temp);
                    sessionkey[i] = temp;
                    i++;
                }
                else if ((temp == 8) || (temp == 127)) //Backspace or delete
                {
                    put_char(temp);
                    i--;
                    sessionkey[i] = 0;
                }
                _delay_ms(1);
            }
            temp = 0;
        }

        put_string("\r\nSend or receive ('S' or 'R'): ");
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
            put_string("\r\nEnter destination: ");
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

            i = 0;

            put_string("\r\nEnter message: ");

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

            put_string("\r\n\r\n");
            put_number(strlen(message));
            put_string(" character long message reads: \r\n");
            put_string(message);

            int error = SendData(dest, message, encryption, sessionkey);
            if (error) put_string("Error sending message");
        }

        else if (mode == 'R')
        {
            uint8_t rmessageflag = 0; //0 if end of message. 1 if stuff still to come
            uint8_t tempflag = 0; //0 if nothing received, 1 if something received

            //TODO pass callsign down layers

            while(1)
            {
                tempflag = RecieveData(&source, message, &rmessageflag, sessionkey);
                if (tempflag == 1) //if something has been received
                {
                    if (rmessageflag == 0)
                    {
                        put_string("\r\n");
                        put_number(strlen(message));
                        put_string(" character long message from ");
                        put_char(source);
                        put_string(" reads: \r\n");
                        put_string(message);
                        return 0; //THIS NEEDS TO BE DELETED TO RECEIVE MORE THAN 1 MESSAGE
                    }
                    else
                    {
                        put_string("\r\nWaiting for next segment\r\n");
                    }
                } 
            }
        }
        //while(1);
       
    }
     return 0;
}