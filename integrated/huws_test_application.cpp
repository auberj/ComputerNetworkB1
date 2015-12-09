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

#include "physical/physical.cpp"
#include "link/link.cpp"
#include "network/network.cpp"
#include "transport/transport.cpp"

//C232HM Pinout
//Yellow - D1
//Orange - D0
//Black - Ground.

void send();
void receive();

int main() {
	_delay_ms(100);  //little delay for the rfm12 to initialize properly
    rfm12_init();    //init the RFM12
    _delay_ms(100);
    sei();
    init_uart0();
    init_timer();
    
	put_string("\r\n\n\n\n\n\n\r\ninitialising.....");
	//send();
	receive();
	
}

void send() {
	char test[] = "This is a test string\0";
	while(1)  {
    	SendPacket('H',test);
	_delay_ms(1000);
	}
}

void receive() {
	char RFrame[30];
	while(1) {
    	if(ReceiveFrame()) {
    		put_string("\r\nPhysical Data Received");
    		RecievePacket(RFrame);
            put_string("\r\n\n\n>>>>>>Received packet: ");
    		put_string(RFrame);
    		for(int i = 0; i < 30; i++) {
    			RFrame[i] = 0;
    		}
    	}
	}
}


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