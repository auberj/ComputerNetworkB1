//#include "class_definitions.h"

#ifndef _MASTER_H_
#define _MASTER_H_
#include "../master_include.h"
#endif

#include "network.h"

//global vars
char neighbours[NumNeighbours] = {0};//all set to 0
char twohops[NumNeighbours*NumNeighbours] = {0}; //list of nodes two hops away
char oldchecksum[NumOldPackets] = {0}; //store old checksums to ensure messages aren't sent multiple times

int main(){
	init_lcd();
    set_orientation(East);
	//display_string("Initialising...\n");

	char source[1] = {0};
	char segment[10] = {0};
	
	//display_string(neighbours);
	sendHello();
	RecieveSegment(source,segment);

	sendHello();
	RecieveSegment(source,segment);
	display_string(segment);
	display_string("\n");
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
	display_string("sending hello...");
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

	for(int i=5;i<(neighbourTableSize+5);i++){
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
	display_string("getting packet...");
	
	int PacketLength; 
	//PacketLength = strlen(packet);

	int PacketType = 0;
	int packetEnd;

	//char packet[MaxPacketLength]; //max packet length in bytes
	RecievePacket(packet);
	/*
	for(int i=127;i>0;i--){
		if(packet[i]!=0){
			packetEnd=i;
			break;
		}
	}*/
	
	PacketLength = strlen(packet);
	//int PacketLength = packetEnd+1;
	display_string("packet recieved: ");
	display_string("packet length: "); display_number(PacketLength); display_string("\n");
	//check packet is intact
	display_string("Packet Checksum: "); display_hex(packet[PacketLength-2],1); display_hex(packet[PacketLength-1],1); display_string("\n");

	uint16_t fullcrc = calcrc(packet, PacketLength-2);

	if((packet[PacketLength-2] != (char)((fullcrc & 0xFF00) >> 8))||(packet[PacketLength-1] != (char)(fullcrc & 0x00FF))){
		display_string("cheksum failed\n");
	}
	display_string("Calc Checksum: "); display_hex((char)((fullcrc & 0xFF00) >> 8),1); display_hex((char)(fullcrc & 0x00FF),1); display_string("...");
	

	char control1 = packet[0];
	char control2 = packet[1];

	//end check packet

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
			else if(control2!=Control2SingleMessage){ 
				PacketType = 4;
			}
			else{ PacketType = 5;} //drop the packet
			
		break;
	}
	display_string("done.\n");
	return PacketType;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int SendSegment(char dest, char* segment){ //provide this to transport layer
	int 	segmentLength = (sizeof(segment)/sizeof(segment[0]));
	char 	packet[segmentLength+7];
	int singleHopFlag = 0;

	for(int i=0;i<NumNeighbours;i++){
		if(dest==neighbours[i]){
			singleHopFlag = 1;
			break; //no need to check the rest of the table
		}
	}

	packet[0] = Control1Message;

	if(singleHopFlag==1){
		packet[1] = 'S';
	}
	else{
		packet[1] = 'X';
	}
	
	packet[2] = SCRADD;
	packet[3] = dest;
	packet[4] = (char)segmentLength;

	for(int i=5;i<(segmentLength+5);i++){
		packet[i] = segment[i-5];
	}

	uint16_t fullcrc = calcrc(packet, segmentLength+5);

	packet[segmentLength+5] = (char)((fullcrc & 0xFF00) >> 8);
	packet[segmentLength+6] = (char)(fullcrc & 0x00FF);

	SendPacket(dest,packet);
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int RecieveSegment(char* source, char* rsegment){ //provide this to transport layer, return 0 if no segment available
	//create variables
	char 	packet[MaxPacketLength] = {0}; //assume max length
	int 	returnval = 0;
	//int 	packetend = 0;
	int repeatPacketFlag;

	int packetType = getPacket(packet); //get packet from DLL
	int PacketLength = strlen(packet);
	//determine if I am intended recipient
	switch (packetType){
		case 1: //recieved a HELLO, send one back!
			sendHello(); //only send this if haven't sent one recently
			processHello(packet);
		break;
		case 2: //details neighbours
			//CODE
		break;

		case 3: //packet is a message for me
			display_string("message for me...\n");
			//extract data from packet
			//source[0] = packet[2]; //source of message
			//detect end of packet

			//int PacketLength = strlen(packet);
			//packetend=PacketLength-1;
			for(int i=(PacketLength-3);i>4;i--){
				rsegment[i-4]=packet[i];
			}
			//copy segment data 
			returnval = 1;
		break;

		case 4: //packet is a message but not for me and not a single hop
			display_string("message not for me...");
			returnval = 0;
			//retransmit packet if not done so before
			repeatPacketFlag = checkRepeatPacket(packet);

			if(repeatPacketFlag!=1){ //if not trasmitted before
				SendPacket(packet[3],packet);
				display_string("retransmit\n");
			}
			else{};
		break;

		case 5:
			//drop the packet
			display_string("Sing hop message not for me...dropped.\n");
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
int	checkRepeatPacket(char* packet){

	int PacketLength = strlen(packet);
	//display_string("CRP: packet length: "); display_number(PacketLength); display_string("\n");
	char checksum1 = packet[PacketLength-2];
	char checksum2 = packet[PacketLength-1];

	display_string("Checksum: "); display_hex(checksum1,1); display_hex(checksum2,1); display_string("\n");

	display_string("check checksum...");
	int duplicateFlag = 0;

	for(int i=0;i<NumOldPackets;i=i+2){
		if((oldchecksum[i]==checksum1)&&(oldchecksum[i+1]==checksum2)){
			duplicateFlag = 1; //already recently transmitted this packet
			display_string("already transmitted\n");
		}
	}
	if(duplicateFlag==0){ //if no duplcates found
		for(int i=NumOldPackets;i>2;i--){ //make space in the table
			oldchecksum[i]=oldchecksum[i-1];
		}
		oldchecksum[0]=checksum1; //load new checksum into the table
		oldchecksum[1]=checksum2;
		display_string("cheksum stored\n");
	}
	display_string("end check checksum\n");
	return duplicateFlag;
}