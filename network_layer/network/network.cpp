//#include "class_definitions.h"

#ifndef _MASTER_H_
#define _MASTER_H_
#include "../master_include.h"
#endif

#include "network.h"

//global vars
char neighbours[NumNeighbours] = {0};//all set to 0

int main(){
	init_lcd();
    set_orientation(East);
	display_string("Initialising...\n");

	
	//display_string(neighbours);
	sendHello();
	gatherNeighbours();
	sendHello();
	gatherNeighbours();
	clear_screen();
	//display_string(neighbours);
	sendNeighbours();
	while(1);
	return 0;
}

void processHello(char* packet){
	char 	neighbour[1];
	int 	neighbourTableSize = (sizeof(neighbours)/sizeof(neighbours[0]));

	getNeighbourAdd(neighbour, packet);

	display_string("processing HELLO\n");
		display_string("neighbour found: ");
		display_char(neighbour[0]); display_string("\n");

		int duplicateFlag=1;
		//check array element is not the same as neighbour address and =0
		for(int i=0;i<neighbourTableSize;i++){
			if(neighbours[i]==neighbour[0]){
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
				if(neighbours[i]==0){
					display_string("space for neighbour\n");
					neighbourSpace = i;
					SpaceFlag = 1;
					break;
				}
				if((i==neighbourTableSize)&&(SpaceFlag==0)){
					display_string("no space for neighbour\n");
				}
			}
			if(SpaceFlag==1){
				neighbours[neighbourSpace] = neighbour[0];
				display_string("neighbour stored in ");
				char str[10];
				sprintf(str, "%d", neighbourSpace);
				display_string(str); display_string("\n");
			}
			
		}
		
	display_string("neighbour processed\n");
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void gatherNeighbours(){
//calculate number of elements in neighbour table
	int 	neighbourTableSize = (sizeof(neighbours)/sizeof(neighbours[0]));
	char 	packet[MaxPacketLength];
	//get responses, check not duplicates 
	char 	neighbour[1];
	int 	packetType = getPacket(packet); //get a new packet

	getNeighbourAdd(neighbour, packet); //extract neighbour address from packet

	if(packetType==1){ //returns 1 for hello packets
		processHello(packet);
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

void sendNeighbours(){
	display_string("sending neighbours\n");
	int 	neighbourTableSize = (sizeof(neighbours)/sizeof(neighbours[0]));
	char 	packet[neighbourTableSize+7];
	packet[0] = Control1Neighbour;
	packet[1] = '0'; //dont care
	packet[2] = SCRADD;
	packet[3] = '0'; //dont care
	packet[4] = (char)(neighbourTableSize & 0x00FF);

	for(int i=5;i<neighbourTableSize;i++){
		packet[i] = neighbours[i-5];
	}
	
	uint16_t fullcrc = calcrc(packet, neighbourTableSize+5);

	packet[neighbourTableSize+5] = (char)((fullcrc & 0xFF00) >> 8);
	packet[neighbourTableSize+6] = (char)(fullcrc & 0x00FF);
	//display_char(packet[neighbourTableSize+6]);
	display_string(packet);
	display_string("\n");
	display_string("done\n");

	return;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void getNeighbourAdd(char* neighbourADD, char* packet){
	display_string("getting neighbour address\n");
	neighbourADD[0] = packet[2];
	return;
}
int getPacket(char* packet){ //gets a packet from DLL and returns its type
	display_string("getting packet\n");
	int PacketType = 0;
	int packetEnd;

	//char packet[MaxPacketLength]; //max packet length in bytes
	RecievePacket(packet);
	for(int i=127;i>0;i--){
		if(packet[i]!=0){
			packetEnd=i;
			break;
		}
	}
	//int PacketLength = (sizeof(packet)/sizeof(packet[0]));
	int PacketLength = packetEnd+1;
	display_string("packet recieved\n");
	char control1 = packet[0];
	char control2 = packet[1];

	/*
	char segment[PacketLength-7];
	for(int i=0;i<(PacketLength-7);i++){
		segment[i]=packet[i+5];
	}*/

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
			if(packet[3]==SCRADD){
				PacketType = 3;
			}
			else{ PacketType = 4;}
			
		break;
	}
	display_string("done.\n");
	return PacketType;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int SendSegment(char dest, char* segment){ //provide this to transport layer
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int RecieveSegment(char* source, char* rsegment){ //provide this to transport layer, return 0 if no segment available
	//create variables
	char 	packet[MaxPacketLength] = {0}; //assume max length
	int 	returnval = 0;
	int 	packetend = 0;

	int packetType = getPacket(packet); //get packet from DLL
	
	//determine if I am intended recipient
	switch (packetType){
		case 1: //recieved a HELLO, send one back!
			sendHello();
			processHello(packet);
		break;
		case 2:
			//CODE
		break;

		case 3: //packet is a message for me
			//extract data from packet
			source[0] = packet[2]; //source of message
			//detect end of packet
			for(int i=127;i>0;i--){
				if(packet[i]!=0){
					packetend=i;
					break;
				}
			}
			//copy segment data 
			for(int i=4;i<packetend;i++){
				rsegment[i-4]=packet[i];
			}
			returnval = 1;
		break;

		case 4: //packet is a message but not for me
			returnval = 0;
		break;
	}
	
	
	return returnval;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint16_t calcrc(char *ptr, int count) //XModem CRC calculator from https://github.com/vinmenn/Crc16
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
    return (crc);
}