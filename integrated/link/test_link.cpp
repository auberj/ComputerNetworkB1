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

#include "../timer/timer.cpp"
#include "../uart/uart.c"

#include "DataLink.h"
#include "link.cpp"
#include "rfm12.h"
#include "rfm12.cpp"

//#include "link/timer/timer.cpp"
//#include "link_dummy/link.cpp"
//#include "network/network.cpp"
//#include "transport/transport.cpp"



//C232HM Pinout. Yellow - D1. Orange - D0. Black - Ground.

void send();
void receive();

int main()
{
    _delay_ms(100);  //little delay for the rfm12 to initialize properly
    rfm12_init();    //init the RFM12
    _delay_ms(100);
    sei();
    init_uart0();
    init_timer();
    put_string("\r\n\r\n\r\n\r\nInitialising...");

    //send();
    receive();
    

}

void send() {
    char random[120] = "hello aaron rowland, this is a string which should get";
    SendPacket(BROADCAST,random);
}

void receive() {
    char Rpacket[125];
    while(1) {
        if(RecievePacket(Rpacket)) {
            put_string("\r\nRecievePacket: ");
            put_string(Rpacket);
        }
        for(int i = 0; i < 125; i++) {
            Rpacket[i] = 0;
        }
    }
}