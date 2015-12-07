#define __PLATFORM_AVR__

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdio.h>
char callsign = 'H';
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
uint16_t calcrc(char *ptr, int count)
{ 
    //put_string("calcCRC\r\n");
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

    //need to avoid 0x00 checksum so that strlen works
    if ((crc >> 8) == 0x00) //If top byte of crc is 0x00
        crc |= 0xFF00; //Set byte to 0xFF

    if (!(crc & 0x00FF)) //if bottom byte of crc is 0x00
        crc |= 0x00FF; //Set bottom byte to 0xFF

    return (crc);
}

int main()
{
    _delay_ms(100);  //little delay for the rfm12 to initialize properly
    rfm12_init();    //init the RFM12
    _delay_ms(100);
    sei();
    init_uart0();
    init_timer();
    put_string("\r\n\r\n\r\n\r\nInitialising...");

    // send();
    // receive();

    char test[50] = "doesnt need byte stuffing";
    struct frame frames[FRAMECOUNT];
    int count = makeframe(&frames, BROADCAST, test, 0);
    
    for(int i = 0; i < count; i++) {
        put_string("\n\n\n\r\ncounter = ");
        put_number(i);
        struct frame receivestuff;
        put_string("\r\nBeforeDecode: ");
        put_string(frames[i].frame);
        decode_frame(&receivestuff, frames[i].frame);
        put_string("\r\ndecoded data: ");
        put_string(receivestuff.data);
    }
    

}

void send() {
    char random[120] = "hello nathan, this is a longer string to test stuff";
    SendPacket(BROADCAST,random);
    // while(1){

    //     put_string("\r\nsend: ");
    //     put_string(random);
    //     rfm12_tx(strlen(random), 0, (uint8_t*)random);
    //     for (uint8_t j = 0; j < 100; j++)   
    //     {   
    //             put_string(". ");
    //             rfm12_tick();   
    //             _delay_us(500); 
    //     }
    //     _delay_ms(500);
    // }
}

void receive() {
    char Rpacket[125];
    while(1) {
        put_char('.');
        if(RecievePacket(Rpacket)) {
            put_string("\r\nRecievePacket: ");
            put_string(Rpacket);
        }
        for(int i = 0; i < 125; i++) {
            Rpacket[i] = 0;
        }
    }
}