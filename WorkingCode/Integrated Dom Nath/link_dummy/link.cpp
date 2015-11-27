#include <stdio.h>

#include "link.h"

#define MAXPACKETSIZE 128

//link

char DummyPacket[MAXPACKETSIZE+1];

int SendPacket(char dest, char* packet){
	put_string("BEGIN SEND PACKET\r\n");
	int PacketLength = strlen(packet);
	for(int i=0;i<(MAXPACKETSIZE+1);i++){
		DummyPacket[i]=0;
	}
	
	//display_string("s packet length: "); display_number(PacketLength); display_string("\n");
	for(int i=0;i<(PacketLength);i++){
		DummyPacket[i]=packet[i];
	}

	put_string("Dom debug");
	put_string(".");
	put_number(strlen(DummyPacket));
	put_string(".");
	for(int i=0;i<(PacketLength);i++){
		put_number(DummyPacket[i]);
		put_string(".");
	}
	//strcpy(DummyPacket,packet);
	put_string("END SEND PACKET\r\n");
	return 0;
}

//int called = 0;

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
	put_string("BEGIN REC PACKET\r\n");
	//strcpy(packet,DummyPacket);
	int PacketLength = strlen(DummyPacket);
	put_string("Dummy packet length: "); put_number(PacketLength); put_string("\r\n");
	//display_string("r packet length: "); display_number(PacketLength); display_string("\n");
	for(int i=0;i<(PacketLength);i++){
		packet[i] = DummyPacket[i];
	}
	for(int i=0;i<(MAXPACKETSIZE+1);i++){
		DummyPacket[i] = 0;
	}
	PacketLength = strlen(packet);
	put_string("Real packet length: "); put_number(PacketLength); put_string("\r\n");
	put_string("END REC PACKET\r\n");
	return 0;
}