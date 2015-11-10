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
	display_string(neighbours);
	sendHello();
	gatherNeighbours(neighbours);
	display_string(neighbours);
	while(1);
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void gatherNeighbours(char* neighbourtable){
//calculate number of elements in neighbour table
	int neighbourTableSize = (sizeof(neighbourtable)/sizeof(neighbourtable[0]));

	//get responses, check not duplicates 
	char neighbour[1];
	int packetType = getPacket(neighbour);
	if(packetType){ //returns 1 for hello packets
		display_string("hello packet detected\n");
		display_string("neighbour found: ");
		display_char(neighbour[0]); display_string("\n");

		int duplicateFlag=1;
		//check array element is not the same as neighbour address and =0
		for(int i=0;i<neighbourTableSize;i++){
			if(neighbourtable[i]==neighbour[0]){
				display_string("duplicate neighbour found, move on.\n");
				duplicateFlag = 0;
			}
		}
		if(duplicateFlag==1){
			display_string("no duplicate neighbour found\n");
			int neighbourSpace;
			int SpaceFlag=0;
			for(int i=0;i<neighbourTableSize;i++){
				//find space for neighbour in table
				if(neighbourtable[i]==0){
					display_string("space for neighbour\n");
					neighbourSpace = i;
					SpaceFlag = 1;
				}
				if((i==neighbourTableSize)&&(SpaceFlag==0)){
					display_string("no space for neighbour\n");
				}
			}
			if(SpaceFlag==1){
				neighbourtable[neighbourSpace] = neighbour[0];
				display_string("neighbour stored in ");
				char str[10];
				sprintf(str, "%d", neighbourSpace);
				display_string(str); display_string("\n");
			}
			
		}
		
	display_string("neighbour processed\n");

	} 
	else{display_string("no hello packet\n");}
	//store in table
	return;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void sendHello(){
	display_string("sending hello\n");
	char packet[HelloPacketLength]; //max packet length in bytes
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
	
	for(int i=5;i<HelloPacketLength-3;i++){
		packet[i] = 'i';
	}
	packet[HelloPacketLength-1] = HelloChecksum;
	packet[HelloPacketLength] = HelloChecksum;
	char destination = 0;
	SendPacket(destination, packet);

	display_string("hello sent\n");
	return;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int getPacket(char* neighbourADD){
	int PacketType = 0;

	char packet[MaxPacketLength]; //max packet length in bytes
	RecievePacket(packet);
	int PacketLength = (sizeof(packet)/sizeof(packet[0]));
	display_string("packet recieved\n");
	char control1 = packet[0];
	char control2 = packet[1];

	neighbourADD[0] = packet[2];

	char DESTADD = packet[3];

	char Length = packet[4];

	char segment[PacketLength-7];
	for(int i=0;i<(PacketLength-7);i++){
		segment[i]=packet[i+5];
	}

	char checkSum1 = packet[PacketLength-1];
	char checkSum2 = packet[PacketLength];
	
	/* if(control1 == Control1Hello){
		PacketType = 1;
		
	}*/

	switch (control1){
		case Control1Hello:
			PacketType = 1;
		break;
		case Control1Neighbour:
			PacketType = 2;
		break;
		case Control1Message:
			PacketType = 3;
		break;
	}
	
	return PacketType;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int SendSegment(char dest, char* segment){ //provide this to transport layer

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int RecieveSegment(char* source, char* rsegment){ //provide this to transport layer, return 0 if no segment available
	//create variables
	char 	packet[MaxPacketLength] = {0}; //assume max length
	int 	returnval;

	RecievePacket(packet); //get packet from DLL
	
	//determine if I am intended recipient
	if((packet[3]==SCRADD)&&(packet[0]==Control1Message)){ //If I am recipient & packet contains a message
		//extract data from packet
		source[0] = packet[2]; //source of message

		returnval = 1;
	}
	else{
		returnval = 0;
	}
	return returnval;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////