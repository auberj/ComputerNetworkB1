//#include "class_definitions.h"

#ifndef _MASTER_H_
#define _MASTER_H_
	#include "../master_include.h"
#endif

#include "network.h"



#define NumNeighbours 10 //can have up to ten neighbours

//gather details of neighbours, store in neighbour table
int neighbours[NumNeighbours]; 

int main(){
	init_lcd();
    set_orientation(East);
	display_string("Initialising...\n");
	sendHello();
	while(1);
	return 0;
}


void gatherNeighbours(int neighbourtable[NumNeighbours]){
	//send hello message

	//get responses, check not duplicates 

	//store in table
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
	
	display_string("hello sent\n");
	char destination = 0;
	SendPacket(destination, packet);
	return;
}




//pass neighbour details to every neighbour (this needs to recur so each node knows about every other node)