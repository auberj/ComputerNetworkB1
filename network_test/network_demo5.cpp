//Author: dm6g13

#define __PLATFORM_AVR__
char callsign;

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdio.h>

#include "network_demo.h"
#include "callsign.h"

#include "timer/timer.cpp"
#include "uart/uart.c"
//#include "uartlink/uartlink.cpp"

#include "link/link.cpp"
#include "network/network.cpp"



//C232HM Pinout
//Yellow - D1
//Orange - D0
//Black - Ground.

int main()
{
    //init_uart1();
    init_network_layer();

    sei();

    init_uart0();
    init_timer();
    
    put_string("\r\nInitialising...");

    //put_char(callsign);

    
    while(1){

        start:
        char dest, source;
        char mode;
        char simulation;


        char temp = '\0'; //temporary character for receiving over uart
        char message[100] = {0};
        char sessionkey[20] = {0};
        uint16_t i = 0;



        put_string("\r\n\r\nWhich simulation to run?:\r\nSimple Message\r\nmultiple by Flood\r\nreceive Multiple from flood\r\nNeighbour simulation\r\nHello simulation\r\nTwo hop simulation\r\nS,F,M,N,H,T?: ");
        while(temp != '\r')
        {
            temp = get_char();
            if (temp >= 32 && temp <= 126)
            {
                put_char(temp);
                simulation = temp;
            }
            else if ((temp == 8) || (temp == 127)) //Backspace or delete
            {
                put_char(temp);
                simulation = 0;
            }
            _delay_ms(1);
        }

        if (simulation == 'S') simulation = 0;
        if (simulation == 'F') simulation = 1;
        if (simulation == 'M') simulation = 2;
        if (simulation == 'N') simulation = 3;
        if (simulation == 'H') simulation = 4;
        if (simulation == 'T') simulation = 5;

        temp = 0;
        char sendas, recas, sendto, num;
        int loopnum;
        char    neighbours1[NumNeighbours];
        char    neighbours2[NumNeighbours];
        
        switch(simulation){

            case 5:
                set_simulation_type(2);
                
                put_string("\r\n\r\nTwo Hop Simulation\r\nMessage Source?: ");
                    while(temp != '\r')
                    {
                        temp = get_char();
                        if (temp >= 32 && temp <= 126)
                        {
                            put_char(temp);
                            sendas = temp;
                        }
                        else if ((temp == 8) || (temp == 127)) //Backspace or delete
                        {
                            put_char(temp);
                            sendas = 0;
                        }
                        _delay_ms(1);
                }
                callsign = sendas;
                temp = 0;


                put_string("\r\nFirst Hop?: ");
                    while(temp != '\r')
                    {
                        temp = get_char();
                        if (temp >= 32 && temp <= 126)
                        {
                            put_char(temp);
                            recas = temp;
                            twohops[0][0] = temp;
                        }
                        else if ((temp == 8) || (temp == 127)) //Backspace or delete
                        {
                            put_char(temp);
                            recas = 0;
                        }
                        _delay_ms(1);
                }
                temp = 0;

                put_string("\r\nMessage Dest?: ");
                    while(temp != '\r')
                    {
                        temp = get_char();
                        if (temp >= 32 && temp <= 126)
                        {
                            put_char(temp);
                            sendto = temp;
                        }
                        else if ((temp == 8) || (temp == 127)) //Backspace or delete
                        {
                            put_char(temp);
                            sendto = 0;
                        }
                        _delay_ms(1);
                }
                
                temp = 0;

               put_string("\r\nEnter message: ");
               i=0;
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

                temp = 0;

                //sendas Q
                //recas  W
                //sendto E

                //start simulation

                put_string("\r\n\r\n*********E >HELLO********\r\n");

                //sendas Q
                //recas  W
                //sendto E

                callsign = sendto; //E says hello to w
                put_string("I am now: ");put_char(callsign);put_string("\r\nNeighbours: ");
                
                put_string(neighbours);
                put_string("\r\n"); put_string("\r\n");
                sendHello();
                //SendSegment(sendto,message); 
                for(int i=0;i<NumNeighbours;i++){
                     neighbours1[i]=neighbours[i]; //original neighbour table state
                }
                put_string("\r\n\r\n******************\r\n");

                callsign = recas; 
                put_string("I am now: ");put_char(callsign);put_string("\r\nNeighbours: ");
                
                put_string(neighbours);
                
                if(RecieveSegment(&source,message)){
                    put_string("\r\n");
                    put_string(message);
                    put_string("\r\n");
                }
                for(int i=0;i<NumNeighbours;i++){
                     neighbours2[i]=neighbours[i];
                }

                put_string("\r\nNeighbours: ");
                
                put_string(neighbours); //W now knows about E

                set_simulation_type(5);
                put_string("\r\n\r\n*******SEND NEIGHBOURS********\r\n");

                callsign = recas; //im W
                for(int i=0;i<NumNeighbours;i++){
                     neighbours[i]=neighbours2[i]; //original neighbour table state
                }
                put_string("I am now: ");put_char(callsign);put_string("\r\nNeighbours: ");
                
                put_string(neighbours);
                put_string("\r\n"); put_string("\r\n");
                
                sendNeighbours();
                //SendSegment(sendto,message); 
                
                put_string("\r\n*******Q PROCECSS NEIGHBOURS******\r\n");
               
                callsign = sendas; //I am now Q
                put_string("I am now: ");put_char(callsign);put_string("\r\n");
                for(int i=0;i<NumNeighbours;i++){
                     neighbours[i]=neighbours1[i];
                }
                if(RecieveSegment(&source,message)){
                    put_string(message);
                    put_string("\r\n");
                }

                put_string("\r\n2hop Neighbours: ");
                
                put_string(twohops[0]);
                put_string("\r\n"); put_string("\r\n");

                put_string("\r\n**********Q SEND***********\r\n");
               
                callsign = sendas; //I am now Q
                put_string("I am now: ");put_char(callsign);put_string("\r\n");

                SendSegment(sendto,message);

                put_string("\r\n****************************\r\n");

                callsign = recas; //i am now W
                put_string("I am now: ");put_char(callsign);put_string("\r\n");
                for(int i=0;i<NumNeighbours;i++){
                     neighbours[i]=neighbours2[i];
                }
                put_string("\r\nNeighbours: ");
                
                put_string(neighbours);put_string("\r\n");
                if(RecieveSegment(&source,message)){
                    put_string(message);
                    put_string("\r\n");
                }

                put_string("\r\n****************************\r\n");

                callsign = sendto; //i am now E
                put_string("I am now: ");put_char(callsign);put_string("\r\n");
                // for(int i=0;i<NumNeighbours;i++){
                //      neighbours2[i]=neighbours[i];
                // }
                if(RecieveSegment(&source,message)){
                    put_string(message);
                    put_string("\r\n");
                }
            break;
        }
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