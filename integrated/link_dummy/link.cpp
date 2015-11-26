#include <stdio.h>

#include "link.h"

//link

char DummyPacket[128] = {0};

int SendPacket(char dest, char* packet){
	int PacketLength = strlen(packet);
	put_string("packet passed to link layer\r\n");
	//display_string("s packet length: "); display_number(PacketLength); display_string("\n");
	for(int i=0;i<(PacketLength);i++){
		DummyPacket[i]=packet[i];
	}
	//strcpy(DummyPacket,packet);
	return 0;
}

int called = 0;

/*
int RecievePacket(char* packet){
	
	//Hello then a message packet
	
	//set SRC address
	if(called==0){
		packet[0] = 'H'; //H for hello
		packet[1] = 'X';
		packet[2] = 'k';
		//set DEST address (doesn't matter for HELLO)
		packet[3] = 0x01;
		//set length
		packet[4] = MaxSegmentLength;
		//set segment to be empty
		for(int i=5;i<15;i++){
			packet[i] = 'i';
		}
		packet[15] = 0x27;
		packet[16] = 0xb7;
	}
	if(called==1){
		packet[0] = 'M'; //M for message
		packet[1] = 'X';
		packet[2] = 'k';
		packet[3] = 'L'; //dest
		packet[4] = 10;
		for(int i = 5;i<15;i++){
			packet[i] = 'm';
		}
		packet[15] = 0xf3;
		packet[16] = 0x7f;
	}
	
	
	
	
	called++;
	return 0;
}


int RecievePacket(char* packet){
	
	//two message packets with the same checksum
	
	//set SRC address
	if(called==0){
		packet[0] = 'M'; //H for hello
		packet[1] = 'X'; //S
		packet[2] = 'k';
		//set DEST address (doesn't matter for HELLO)
		packet[3] = 'L';
		//set length
		packet[4] = 10;
		//set segment to be empty
		for(int i=5;i<15;i++){
			packet[i] = 'm';
		}
		packet[15] = 0xf3;//0x95;
		packet[16] = 0x7f;//0xb7;
	}
	if(called==1){
		packet[0] = 'M'; //M for message
		packet[1] = 'X';
		packet[2] = 'k';
		packet[3] = 'L'; //dest
		packet[4] = 10;
		for(int i = 5;i<15;i++){
			packet[i] = 'm';
		}
		packet[15] = 0xd3; //f3
		packet[16] = 0x7f;
	}
	
	
	
	

	//int PacketLength = strlen(packet);
	//display_string("packet length: "); display_number(PacketLength); display_string("\n");
	//display_string("Checksum: "); display_hex(packet[16],1); display_hex(packet[17],1); display_string("\n");
	called++;
	return 0;
}*/

int RecievePacket(char* packet){
	
	//strcpy(packet,DummyPacket);
	int PacketLength = strlen(DummyPacket);
	//display_string("r packet length: "); display_number(PacketLength); display_string("\n");
	for(int i=0;i<(PacketLength);i++){
		packet[i] = DummyPacket[i];
	}
	return 0;
}