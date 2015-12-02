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
#include "../physical/physical.cpp"

#include "DataLink.h"
#include "link.cpp"
//#include "../physical/rfm12.h"

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
    //receive();

    char test[50] = "this is a long string which will be split";
    struct frame frames[FRAMECOUNT];
    int count = makeframe(&frames, BROADCAST, test, 0);
    struct frame receivestuff;
    for(int i = 0; i < count; i++) {
        put_string("\r\nBeforeDecode: ");
        put_string(frames[i].frame);
        decode_frame(&receivestuff, frames[i].frame);
        put_string("\r\ndecoded data: ");
        put_string(receivestuff.data);
    }
    

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