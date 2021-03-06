//Author: dm6g13

#define __PLATFORM_AVR__

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdio.h>

#include "application.h"
#include "callsign.h"

#include "timer/timer.cpp"
#include "uart/uart.c"

#ifdef transporttest

    #include "transporttest/transporttest.cpp"

#else

    #include "physical/physical.cpp"

    #ifdef uartlink
        #include "uartlink/uartlink.cpp"
    #else
        #include "link/link.cpp"
    #endif

    #ifdef networklink
        #include "networklink/networklink.cpp"
    #else
        #include "network/network.cpp"
    #endif

#endif


//#include "transport/transport.cpp"

//C232HM Pinout
//Yellow - D1
//Orange - D0
//Black - Ground.

int main()
{
    _delay_ms(100);  //little delay for the rfm12 to initialize properly
    #ifndef transporttest
    #ifdef uartlink
    init_uart1();
    #else
    rfm12_init();    //init the RFM12
    init_network_layer();
    #endif
    _delay_ms(100);
    sei();
    #endif
    init_uart0();
    init_timer();
    
    put_string("\r\n\r\n\r\n\r\nInitialising...");

    //put_char(callsign);

    
    while(1){

        start:
        char dest, source;
        char mode;
        char encryption;

        char temp = '\0'; //temporary character for receiving over uart
        char message[100] = {0};
        char sessionkey[20] = {0};
        uint16_t i = 0;



        // put_string("\r\n\r\nEncrypt session ('Y' or 'N'): ");
        // while(temp != '\r')
        // {
        //     temp = get_char();
        //     if (temp >= 32 && temp <= 126)
        //     {
        //         put_char(temp);
        //         encryption = temp;
        //     }
        //     else if ((temp == 8) || (temp == 127)) //Backspace or delete
        //     {
        //         put_char(temp);
        //         encryption = 0;
        //     }
        //     _delay_ms(1);
        // }

        // if (encryption == 'Y') encryption = 1;
        // if (encryption == 'N') encryption = 0;

        temp = 0;

        // if (encryption)
        // {
        //     put_string("\r\nEnter session key: ");
        //     while(temp != '\r')
        //     {
        //         temp = get_char();
        //         if (temp >= 32 && temp <= 126)
        //         {
        //             put_char(temp);
        //             sessionkey[i] = temp;
        //             i++;
        //         }
        //         else if ((temp == 8) || (temp == 127)) //Backspace or delete
        //         {
        //             put_char(temp);
        //             i--;
        //             sessionkey[i] = 0;
        //         }
        //         _delay_ms(1);
        //     }
        //     temp = 0;
        // }

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

            #ifdef transporttest
                globalsegmentnumber = 0;
                for (int i = 0; i < MAXSEGMENTS; i++)
                {
                    int j = 0;
                    while(globalpacket[j][i])
                    {
                        globalpacket[j][i] = 0;
                        j++;
                    }
                }
            #endif

            int error = SendSegment(dest, message);
            if (error) put_string("\r\n\r\nError sending message");
        }

        else if (mode == 'R')
        {
            uint8_t rmessageflag = 0; //0 if end of message. 1 if stuff still to come
            uint8_t tempflag = 0; //0 if nothing received, 1 if something received

            //TODO pass callsign down layers

            #ifdef transporttest
            globalsegmentnumber = 0;
            #endif   
            //char* dummy
            while(1)
            {
                tempflag = RecieveSegment(&source, message);
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
                        goto start; //THIS NEEDS TO BE DELETED TO RECEIVE MORE THAN 1 MESSAGE
                    }
                    else
                    {
                        put_string("\r\nWaiting for next segment");
                    }
                } 
            }
        }
        //while(1);
       
    }
     return 0;
}

//XModem CRC calculator from https://github.com/vinmenn/Crc16
uint16_t calcrc(char *ptr, int count)
{ 
    int  crc;
    char i;
    crc = 0;
    while (--count >= 0)
    {
        crc = crc ^ (int) *ptr++ << 8;
        i = 8;
        do
        {
            if (crc & 0x8000)
                crc = crc << 1 ^ 0x1021;
            else
                crc = crc << 1;
        } while(--i);
    }

    //below was added by dm6g13
    //need to avoid 0x00 checksum so that strlen works
    if ((crc >> 8) == 0x00) //If top byte of crc is 0x00
        crc |= 0xFF00; //Set top byte to 0xFF

    if (!(crc & 0x00FF)) //if bottom byte of crc is 0x00
        crc |= 0x00FF; //Set bottom byte to 0xFF

    return (crc);
}