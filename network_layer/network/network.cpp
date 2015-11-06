//#include "class_definitions.h"

#ifndef _MASTER_H_
#define _MASTER_H_
	#include "../master_include.h"
#endif

#include "network.h"

//gather details of neighbours, store in neighbour table


int main(){
	init_lcd();
    set_orientation(East);
	display_string("Initialising...\n");

	char neighbours[NumNeighbours] = {0};//all set to 0
	gatherNeighbours(neighbours);
	while(1);
	return 0;
}


void gatherNeighbours(char* neighbourtable){
	//send hello message
	sendHello();

	//get responses, check not duplicates 
	char neighbour[1];
	int packetType = getPacket(neighbour);
	if(packetType){ //returns 1 for hello packets
		//check array element is not the same as neighbour address and =0
		display_char(neighbour[0]);
	} 

	//store in table
	return;
}

void sendHello(){
	display_string("sending hello\n");
	char packet[128]; //max packet length in bytes
	//set control bits
	packet[0] = Control1Hello;
	packet[1] = Control1Hello;
	//set SRC address
	packet[2] = SCRADD;
	//set DEST address (doesn't matter for HELLO)
	packet[3] = 0x00;
	//set length
	packet[4] = MaxSegmentLength;
	//set segment to be empty
	for(int i=5;i<125;i++){
		packet[i] = 'i';
	}
	packet[126] = HelloChecksum;
	packet[127] = HelloChecksum;
	char destination = 0;
	SendPacket(destination, packet);

	display_string("hello sent\n");
	
	return;
}

int getPacket(char* neighbourADD){
	int PacketType = 0;
	char packet[128]; //max packet length in bytes
	RecievePacket(packet);

	char control1 = packet[0];
	char control2 = packet[1];

	neighbourADD[0] = packet[2];

	char DESTADD = packet[3];

	char Length = packet[4];

	char segment[MaxSegmentLength];
	for(int i=0;i<MaxSegmentLength;i++){
		segment[i]=packet[i+5];
	}

	char checkSum1 = packet[126];
	char checkSum2 = packet[127];

	if(control1 = Control1Hello){
		PacketType = 1;
	}
	return PacketType;
}



//pass neighbour details to every neighbour (this needs to recur so each node knows about every other node)