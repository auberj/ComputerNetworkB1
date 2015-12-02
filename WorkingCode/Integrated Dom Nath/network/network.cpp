//#include "class_definitions.h"

//control bit definitions
#define Control1Hello 'H'
#define Control2Hello 'O'

#define Control1Neighbour 'N'

#define Control1Message 'M'
#define Control2SingleMessage 'S'

//source address (my address)
#define SCRADD 'N'

#define MaxSegmentLength 0x79

//checksum definitions
#define HelloChecksum 0x00

#define NumNeighbours 10 //can have up to ten neighbours

#define HelloPacketLength 15
#define MaxPacketLength 128

#define NumOldPackets 20 //the number of old packets to check for retransmission multiplied by 2

//network message frequencies
#define HELLOFREQ 10

#include "network.h"
#include <stdio.h>
//global vars
char neighbours[NumNeighbours] = {0};//all set to 0
char twohops[NumNeighbours*NumNeighbours] = {0}; //list of nodes two hops away
char oldchecksum[NumOldPackets] = {0}; //store old checksums to ensure messages aren't sent multiple times

void processHello(char* packet){
	char 	neighbour[1];
	int 	neighbourTableSize = (sizeof(neighbours)/sizeof(neighbours[0]));

	getNeighbourAdd(neighbour, packet);

	put_string("processing HELLO\r\n");
		put_string("neighbour found: ");
		put_char(neighbour[0]); put_string("\r\n");

		int duplicateFlag=1;
		//check array element is not the same as neighbour address and =0
		for(int i=0;i<neighbourTableSize;i++){
			if(neighbours[i]==neighbour[0]){
				put_string("duplicate neighbour found, move on.\r\n");
				duplicateFlag = 0;
			}
		}
		if(duplicateFlag==1){
			put_string("no duplicate neighbour found\r\n");
			int neighbourSpace;
			int SpaceFlag=0;
			for(int i=0;i<neighbourTableSize;i++){
				//find space for neighbour in table
				if(neighbours[i]==0){
					put_string("space for neighbour\r\n");
					neighbourSpace = i;
					SpaceFlag = 1;
					break;
				}
				if((i==neighbourTableSize)&&(SpaceFlag==0)){
					put_string("no space for neighbour\r\n");
				}
			}
			if(SpaceFlag==1){
				neighbours[neighbourSpace] = neighbour[0];
				put_string("neighbour stored in ");
				char str[10];
				sprintf(str, "%d", neighbourSpace);
				put_string(str); put_string("\r\n");
			}
			
		}
		
	put_string("neighbour processed\r\n");
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
	else{put_string("no hello packet\r\n");}
	//store in table
	return;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void sendHello(){
	put_string("sending hello...");
	char packet[16] = {0}; //max packet length in bytes
	//set control bits
	packet[0] = Control1Hello;
	packet[1] = Control1Hello;
	//set SRC address
	packet[2] = SCRADD;
	//set DEST address (doesn't matter for HELLO)
	packet[3] = 0xFF;
	//set length
	packet[4] = 8;
	//set segment to be empty
	
	for(int i=5;i<13;i++){
		packet[i] = 'i';
	}
	packet[13] = 0xcd;
	packet[14] = 0x69;
	char destination = 0;
	int PacketLength = strlen(packet);
	put_hex(packet[15],1);
	put_string("h packet length: "); put_number(PacketLength); put_string("\r\n");

	SendPacket(destination, packet);

	put_string("hello sent\r\n");
	return;
}

void sendNeighbours(){
	put_string("sending neighbours\r\n");
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
	//put_char(packet[neighbourTableSize+6]);
	put_string(packet);
	put_string("\r\n");
	put_string("done\r\n");

	return;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void getNeighbourAdd(char* neighbourADD, char* packet){
	put_string("getting neighbour address\r\n");
	neighbourADD[0] = packet[2];
	return;
}
int getPacket(char* packet){ //gets a packet from DLL and returns its type
	put_string("\r\nBEGIN GET PACKET\r\n");
	
	int PacketLength; 
	//PacketLength = strlen(packet);

	int PacketType = 1;
	int packetEnd;

	//char packet[MaxPacketLength]; //max packet length in bytes
	RecievePacket(packet);
	
	PacketLength = strlen(packet);
	//int PacketLength = packetEnd+1;
	put_string("packet recieved: ");
	put_string("packet length: "); put_number(PacketLength); put_string("\r\n");
	//check packet is intact
	put_string("Packet Checksum: "); put_hex(packet[PacketLength-2],1); put_string(" "); put_hex(packet[PacketLength-1],1); put_string("\r\n");

	uint16_t fullcrc = calcrc(packet, PacketLength-2);

	if((packet[PacketLength-2] != (char)((fullcrc & 0xFF00) >> 8))||(packet[PacketLength-1] != (char)(fullcrc & 0x00FF))){
		put_string("cheksum failed\r\n");
		PacketType = 0;
	}
	put_string("Calc Checksum: "); put_hex((char)((fullcrc & 0xFF00) >> 8),1);put_string(" "); put_hex((char)(fullcrc & 0x00FF),1); put_string("...");
	
	if(PacketType!=0){
		char control1 = packet[0];
		char control2 = packet[1];

		switch (control1){
			case Control1Hello:
				PacketType = 1;
			break;
			case Control1Neighbour:
				PacketType = 2;
			break;
			case Control1Message:
				if(packet[3]==SCRADD){
					PacketType = 3; //message is for me
				}
				else if(control2!=Control2SingleMessage){ 
					PacketType = 4; //message is not a single hop and not for me
				}
				else{ PacketType = 5;} //drop the packet
				
			break;
		}
	}
	
	put_string("done.\r\n");
	put_string("\r\nEND GET PACKET\r\n");
	return PacketType;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int SendSegment(char dest, char* segment){ //provide this to transport layer
	put_string("\r\nBEGIN SEND SEGMENT\r\n");
	int 	sendcounter;
	int 	segmentLength = strlen(segment);
	put_string("\r\nsegment length: ");put_number(segmentLength);put_string("\r\n");
	char 	packet[MaxPacketLength] = {0}; //only 7 other bits but need a null!
	int 	packetLength = strlen(packet);
	put_string("1. packet length: ");put_number(packetLength);put_string("\r\n");

	//char 	packet[segmentLength+8]; //only 7 other bits but need a null!

	int singleHopFlag = 0;

	packetLength = strlen(packet);
	put_string("2. packet length: ");put_number(packetLength);put_string("\r\n");
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

	put_string("Copying in ");put_number(segmentLength);put_string(" segment bytes\r\n");
	for(int i=0;i<segmentLength;i++){
		packet[i+5] = segment[i];
	}
	packetLength = strlen(packet);
	put_string("3. packet length: ");put_number(packetLength);put_string("\r\n");
	uint16_t fullcrc = calcrc(packet, packetLength);

	packet[packetLength] = (char)((fullcrc & 0xFF00) >> 8);
	packet[packetLength+1] = (char)(fullcrc & 0x00FF);
	//packet[packetLength] = '\0';
	packetLength = strlen(packet);
	put_string("4. packet length: ");put_number(packetLength);put_string("\r\n");

	SendPacket(dest,packet);
	put_string("\r\nEND SEND SEGMENT\r\n");
	if(sendcounter==HELLOFREQ){
		sendHello();
		sendcounter=0;
	}
	sendcounter++;
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int RecieveSegment(char* source, char* rsegment){ //provide this to transport layer, return 0 if no segment available
	put_string("\r\nBEGIN RECEIVE SEGMENT\r\n");
	//create variables
	int 	recievecounter;
	char 	packet[MaxPacketLength] = {0}; //assume max length
	int 	returnval = 0;
	//int 	packetend = 0;
	int repeatPacketFlag;

	int packetType = getPacket(packet); //get packet from DLL
	int PacketLength = strlen(packet);
	//determine if I am intended recipient
	switch (packetType){
		case 0:
			put_string("packet corrupted, dropped.\r\n");
		break;
		case 1: //recieved a HELLO, send one back!
			if(recievecounter==HELLOFREQ){
				sendHello();
				recievecounter=0;
			}
			recievecounter++;
			processHello(packet);
		break;
		case 2: //details neighbours
			//CODE
		break;

		case 3: //packet is a message for me

			put_string("message for me. packet length: "); put_number(PacketLength); put_string("\r\n");
			put_string("\r\n");
			put_number(strlen(rsegment));
			put_string("\r\n");

			for(int i=5;i<(PacketLength-2);i++){
				rsegment[i-5]=packet[i];
			}

			put_number(strlen(rsegment));
			put_string("\r\n");
			//rsegment[PacketLength-7] = '\0';

			//copy segment data 
			returnval = 1;
		break;

		case 4: //packet is a message but not for me and not a single hop
			put_string("message not for me.\r\n");
			returnval = 0;
			//retransmit packet if not done so before
			repeatPacketFlag = checkRepeatPacket(packet);

			if(repeatPacketFlag!=1){ //if not trasmitted before
				put_string("retransmiting.\r\n");
				SendPacket(packet[3],packet);
			}
			else{put_string("already retransmitted\r\n");};
		break;

		case 5:
			//drop the packet
			put_string("Single hop message not for me...dropped.\r\n");
		break;
	}
	
	put_string("\r\nEND RECEIVE SEGMENT\r\n");
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

    if ((crc >> 8) == 0x00) //If top byte of crc is 0x00
    	crc |= 0xFF00; //Set byte to 0xFF

    if (!(crc & 0x00FF)) //if bottom byte of crc is 0x00
    	crc |= 0x00FF; //Set bottom byte to 0xFF

    return (crc);
}
int	checkRepeatPacket(char* packet){
	put_string("check checksum,");

	int PacketLength = strlen(packet);
	//put_string("CRP: packet length: "); put_number(PacketLength); put_string("\r\n");
	char checksum1 = packet[PacketLength-2];
	char checksum2 = packet[PacketLength-1];

	put_string("Checksum: "); put_hex(checksum1,1); put_string(" "); put_hex(checksum2,1); put_string("\r\n");

	int duplicateFlag = 0;

	for(int i=0;i<NumOldPackets;i=i+2){
		if((oldchecksum[i]==checksum1)&&(oldchecksum[i+1]==checksum2)){
			duplicateFlag = 1; //already recently transmitted this packet
			put_string("already transmitted,");
		}
	}
	if(duplicateFlag==0){ //if no duplcates found
		for(int i=NumOldPackets;i>2;i--){ //make space in the table
			oldchecksum[i]=oldchecksum[i-1];
		}
		oldchecksum[0]=checksum1; //load new checksum into the table
		oldchecksum[1]=checksum2;
		put_string("cheksum stored,");
	}
	put_string("END.\r\n");
	return duplicateFlag;
}

void	displaySegment(char* packet){
	int PacketLength = strlen(packet);
	char segment[PacketLength-7];

	for(int i=5;i<(PacketLength-2);i++){
		segment[i-5]=packet[i];
	}

	put_string(segment);
}