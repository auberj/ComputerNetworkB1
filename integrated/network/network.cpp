//#include "class_definitions.h"

//control bit definitions
#define Control1Hello 'H'
#define Control2Hello 'O'

#define Control1Neighbour 'N'

#define Control1Message 'M'
#define Control2SingleMessage 'S'
#define Control2FloodMessage 'F'
#define Control2DoubleHop 'D'

#define DLLFLOOD 0xFF //address to send to DLL when I want everyone to get the packet

#define MaxSegmentLength 0x79
#define HelloTimeout 500 //milliseconds
//checksum definitions
#define HelloChecksum 0x00

#define NumNeighbours 10 //can have up to ten neighbours

#define HelloPacketLength 15
#define MaxPacketLength 128



#define NumOldPackets 20 //the number of old packets to check for retransmission multiplied by 2

#include "network.h"
#include <stdio.h>
//global vars
char 	neighbours[NumNeighbours] = {0};//all set to 0
char 	twohops[NumNeighbours][NumNeighbours+1] = {0}; //list of nodes two hops away [neighbour][2hops]
char 	oldchecksum[NumOldPackets] = {0}; //store old checksums to ensure messages aren't sent multiple times
char 	oldchecksumrecieved[NumOldPackets] = {0};
int 	oldTime;

//provide to transport layer:
int SendSegment(char dest, char* segment){ //provide this to transport layer
	put_string("\r\nBEGIN SEND SEGMENT\r\n");
	periodicHello();
	int 	segmentLength = strlen(segment);
	put_string("\r\nsegment length: ");put_number(segmentLength);put_string("\r\n");
	char 	packet[MaxPacketLength] = {0}; //only 7 other bits but need a null!
	int 	packetLength = strlen(packet);
	displayPacket(packet,1);

	//char 	packet[segmentLength+8]; //only 7 other bits but need a null!

	int 	singleHopFlag = 0;
	int 	doubleHopFlag = 0;
	char 	dlladdress;

	packetLength = strlen(packet);
	displayPacket(packet,1);
	for(int i=0;i<NumNeighbours;i++){
		if(dest==neighbours[i]){
			singleHopFlag = 1;
			break; //no need to check the rest of the table
		}
	}

	packet[0] = Control1Message;

	if(singleHopFlag==1){
		packet[1] = 'S';
		dlladdress = 'dest';
		put_string("single hop flag\r\n");
	}
	else if(doubleHopFlag==1){
		packet[1] = 'D';
		put_string("double hop flag\r\n");
		dlladdress = calcNextHop(dest);
		// TO DO:
		// -set up to know what the next hop is 

	}
	else{
		packet[1] = 'F';
		put_string("flood\r\n");
		dlladdress = DLLFLOOD;
	}
	
	packet[2] = callsign;
	packet[3] = dest;
	packet[4] = (char)segmentLength;

	put_string("Copying in ");put_number(segmentLength);put_string(" segment bytes\r\n");
	for(int i=0;i<segmentLength;i++){
		packet[i+5] = segment[i];
	}
	packetLength = strlen(packet);
	displayPacket(packet,1);
	uint16_t fullcrc = calcrc(packet, packetLength);

	packet[packetLength] = (char)((fullcrc & 0xFF00) >> 8);
	packet[packetLength+1] = (char)(fullcrc & 0x00FF);
	//packet[packetLength] = '\0';
	packetLength = strlen(packet);
	displayPacket(packet,1);
	put_string("***passing to DLL***\r\n");
	put_string(packet);put_string("\r\n");
	SendPacket(dlladdress,packet);
	put_string("***return from DLL***\r\n");
	put_string("\r\nEND SEND SEGMENT\r\n");
	return 0;
}
int RecieveSegment(char* source, char* rsegment){ //provide this to transport layer, return 0 if no segment available
	put_string("\r\nBEGIN RECEIVE SEGMENT\r\n");
	//create variables
	char 	packet[MaxPacketLength] = {0}; //assume max length
	int 	returnval = 0;
	//int 	packetend = 0;
	int repeatPacketFlag;
	int neighbourFlag;

	int packetType = getPacket(packet); //get packet from DLL
	int PacketLength = strlen(packet);
	//determine if I am intended recipient
	switch (packetType){
		case 0:
		put_string("packet corrupted, dropped.\r\n");
			returnval = 0; 								//nothing for TRAN layer
			break;

		case 1: 										//recieved a HELLO, send one back!
			periodicHello(); 							//only sends a hello every 500ms
			processHello(packet);
			returnval = 0;								//nothing for TRAN layer
			break;

		case 2: 										//Packet contains details of neighbours
		processNeighbours(packet);
			returnval = 0;								//nothing for TRAN layer
			break;

		case 3: 										//packet is a message for me

		put_string("message for me. packet length: "); put_number(PacketLength); put_string("\r\n");
		repeatPacketFlag = checkRecievedPacket(packet);
			if(repeatPacketFlag==0){ 					//if not recieved before (prevents multiple packets being received in a flood)
				returnval = 1;
				put_string("Not seen this packet before\r\n");
			}
			else {
				returnval = 0;							//ELSE nothing for TRAN layer
				put_string("Packet dropped\r\n");
			}
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

		case 4: 										//packet is a message but not for me and is a double hop
		put_string("message not for me.\r\n");


		repeatPacketFlag = checkRepeatPacket(packet);
		neighbourFlag = isANeighbour(packet);

			if((repeatPacketFlag==0)&&(neighbourFlag==1)){ //if not trasmitted before and a neighbour
				put_string("retransmiting packet: ");
				put_string(packet);put_string("\r\n");
				SendPacket(packet[3],packet); //destination address is used here as messages can only be sent 2 hops directly
			}
			else{put_string("already retransmitted\r\n");};
			returnval = 0; //nothing to pass up to TRAN layer
			break;

		case 5: 										//drop the packet
		put_string("Single hop message not for me...dropped.\r\n");
		returnval = 0;
		break;

		case 6: 										//there was no packet
		put_string("no packet.\r\n");
		returnval = 0;
		break;

		case 7:											//flood the packet
		repeatPacketFlag = checkRepeatPacket(packet);
			if(repeatPacketFlag==0){ //if not trasmitted before and a neighbour
				put_string("flooding packet: ");
				put_string(packet);put_string("\r\n");
				SendPacket(DLLFLOOD,packet); //flood it
			}
		break;
	}

	put_string("\r\nEND RECEIVE SEGMENT\r\n");
	return returnval;
}
void processHello(char* packet){
	char 	neighbour[1];
	//int 	NumNeighbours = (sizeof(neighbours)/sizeof(neighbours[0]));

	getNeighbourAdd(neighbour, packet);

	put_string("processing HELLO\r\n");
	put_string("neighbour found: ");
	put_char(neighbour[0]); put_string("\r\n");

	int duplicateFlag=1;
		//check array element is not the same as neighbour address and =0
	for(int i=0;i<NumNeighbours;i++){
		if(neighbours[i]==neighbour[0]){
			put_string("duplicate neighbour found, move on.\r\n");
			duplicateFlag = 0;
		}
	}
	if(duplicateFlag==1){
		put_string("no duplicate neighbour found\r\n");
		int neighbourSpace;
		int SpaceFlag=0;
		for(int i=0;i<NumNeighbours;i++){
				//find space for neighbour in table
			if(neighbours[i]==0){
				put_string("space for neighbour\r\n");
				neighbourSpace = i;
				SpaceFlag = 1;
				break;
			}
			if((i==NumNeighbours)&&(SpaceFlag==0)){
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
void periodicHello(){
		int 	currentTime = millis();

		if(currentTime>(oldTime+HelloTimeout)){
			sendHello();
		}

		oldTime = currentTime;
		return;
}
int isANeighbour(char* address){ //returns 1 if the person is a neighbour, 0 if not
	int neighbourFlag = 0;

	for(int i=0;i<NumNeighbours;i++){
		if(neighbours[i]==address[3]){
			put_string("This person is a neighbour\r\n");
			neighbourFlag = 1;
			break;
		}
	}
	if(neighbourFlag==0){
		put_string("This person is NOT a neighbour\r\n");
	}
	return neighbourFlag;
}
void sendHello(){
	put_string("sending hello...");
	char packet[MaxPacketLength] = {0}; //max packet length in bytes
	//set control bits
	packet[0] = Control1Hello;
	packet[1] = Control1Hello;
	//set SRC address
	packet[2] = callsign;
	//set DEST address
	packet[3] = DLLFLOOD;
	//set length
	packet[4] = 8;
	//set segment to be empty
	
	for(int i=5;i<13;i++){
		packet[i] = 'i';
	}
	displayPacket(packet,1);
	int 	packetLength = strlen(packet);
	uint16_t fullcrc = calcrc(packet, packetLength);

	packet[packetLength] = (char)((fullcrc & 0xFF00) >> 8);
	packet[packetLength+1] = (char)(fullcrc & 0x00FF);
	
	//char destination = 0xFF; //this is to flood a packet
	int PacketLength = strlen(packet);
	put_hex(packet[15],1);
	displayPacket(packet,1);
	put_string("passing to DLL\r\n");
	put_string(packet);put_string("\r\n");
	SendPacket(DLLFLOOD, packet);
	put_string("returning from DLL\r\n");
	put_string("hello sent\r\n");
	return; //done
}
void sendNeighbours(){
	put_string("SENDING NEIGHBOURS\r\n");
	
	char 	packet[NumNeighbours+8] = {'0'};
	packet[0] = Control1Neighbour;
	packet[1] = 'X'; //dont care
	packet[2] = callsign;
	packet[3] = 'X'; //dont care
	packet[4] = (char)(NumNeighbours);

	for(int i=5;i<(NumNeighbours+5);i++){
		packet[i] = neighbours[i-5];
	}
	
	uint16_t fullcrc = calcrc(packet, NumNeighbours+5);

	packet[NumNeighbours+6] = (char)((fullcrc & 0xFF00) >> 8);
	packet[NumNeighbours+7] = (char)(fullcrc & 0x00FF);
	//put_char(packet[NumNeighbours+6]);
	put_string("passing to DLL\r\n");
	put_string(packet);put_string("\r\n");
	SendPacket(DLLFLOOD, packet);
	put_string("return from DLL\r\n");
	put_string(packet);
	put_string("\r\n");
	put_string("END SENDING NEIGHBOURS\r\n");
	return; //done
}
void processNeighbours(char* packet){ //processes a packet detailing neighbours (ie, 2 hops away)
	put_string("PROCESSING 2HOP NEIGHBOURS\r\n");
	int 	PacketLength = strlen(packet);
	char 	onehopadd = packet[2]; //the neighbour that sent the packet
	char 	twohoparray[NumNeighbours];

	for(int i=0;i<NumNeighbours;i++){
		twohoparray[i] = packet[i+5]; //twohoparray now contains details of neighbour's neighbours
	}

	int twohopsrow = 0;
	int SpaceFlag = 0;
	for(int i=0;i<NumNeighbours;i++){ //find space in this array
		if(twohops[i][0]=='0'){
			twohopsrow = i;
			SpaceFlag = 1;
			twohops[i][0] = onehopadd;
			put_string("1hop neighbour: ");put_char(onehopadd);put_string(" stoerd in: ");put_number(i);put_string("\r\n");
			break;
		}
	}
	if(SpaceFlag==1){
		put_string("now putting 2hop neighbours in\r\n");
		for(int i=0;i<NumNeighbours;i++){
			twohops[twohopsrow][i+1] = twohoparray[i];
		}
		put_string("done putting 2hop neighbours in\r\n");
	}
	put_string("END PROCESSING 2HOP NEIGHBOURS\r\n");
	return;
}
void processDoubleHop(char* packet){ //processes a double packet that is not for me
	int 	PacketLength = strlen(packet);
	return;
}
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
	int continueflag = RecievePacket(packet);
	put_string(packet);put_string("\r\n");
	
	if(continueflag==1){
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
				if(packet[3]==callsign){
						PacketType = 3; //message is for me
					}
					else{ //if message is not for me then....
						switch(control2){
							case Control2SingleMessage: //message is a single hop
								PacketType = 5; 		//drop it
								break;

								case Control2DoubleHop:
								PacketType = 4; //message is a double hop but not for me
								break;

							case Control2FloodMessage: //message is to be flooded
							PacketType = 7;
							break;

							default:
								PacketType = 7;	// if in doubt, flood it
								break;
							}
						}

						break; 

						default:
					PacketType = 6; //no readable packet
					break;
				}
			}

		}
		else{
		PacketType = 6; //no packet
	}
	//put_string("done.\r\n");
	put_string("\r\nEND GET PACKET. Packet Type: ");put_number(PacketType);put_string("\r\n");
	return PacketType;
}
char calcNextHop(char dest){ //returns the next node to send data to
	put_string("\r\nBEGIN CALC NEXT HOP\r\n");
	char 	nextHop;
	int 	foundHopFlag = 0;
	for(int i=0;i<NumNeighbours;i++){
		for(int j=1;j<(NumNeighbours+1);j++){
			if(twohops[i][j]==dest){
				nextHop = twohops[i][0];
				put_string("next hop is: ");put_char(nextHop);put_string("\r\n");
				foundHopFlag = 1;
				break;
			}
			
		}
		if(foundHopFlag==1){
			break;
		}
	}
	if(foundHopFlag==0){ //if i cant work it out then flood the packet
		nextHop = 0xFF;
		put_string("couldn't find next hop. flood.\r\n");
	}
	put_string("\r\nEND CALC NEXT HOP\r\n");
	return nextHop;
}
uint16_t calcrc(char *ptr, int count){ //XModem CRC calculator from https://github.com/vinmenn/Crc16
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
int	checkRepeatPacket(char* packet){ //for sending packets in a flood, return 0 if not a duplicate
	put_string("Checking repeat packet");

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
int	checkRecievedPacket(char* packet){ //for recieving a flooded packet
	put_string("Checking if packet has already been recieved.\r\n");

	int PacketLength = strlen(packet);
	//put_string("CRP: packet length: "); put_number(PacketLength); put_string("\r\n");
	char checksum1 = packet[PacketLength-2];
	char checksum2 = packet[PacketLength-1];

	put_string("Checksum: "); put_hex(checksum1,1); put_string(" "); put_hex(checksum2,1); put_string("\r\n");

	int duplicateFlag = 0;

	for(int i=0;i<NumOldPackets;i=i+2){
		if((oldchecksumrecieved[i]==checksum1)&&(oldchecksumrecieved[i+1]==checksum2)){
			duplicateFlag = 1; //already recently transmitted this packet
			put_string("already recieved");
		}
	}
	if(duplicateFlag==0){ //if no duplcates found
		for(int i=NumOldPackets;i>2;i--){ //make space in the table
			oldchecksumrecieved[i]=oldchecksumrecieved[i-1];
		}
		oldchecksumrecieved[0]=checksum1; //load new checksum into the table
		oldchecksumrecieved[1]=checksum2;
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
void	displayPacket(char* packet,int command){ //1:Dsiaply packet length, 2: display whole packet
	int 	packetLength = strlen(packet);

	switch(command){
		case 1:
			put_string("1. packet length: ");put_number(packetLength);put_string("\r\n");
		break;

		case 2:
			for(int i=0;i<(packetLength-2);i++){
				put_char(packet[i]);
			}
			put_hex(packet[packetLength-2],2);put_string("\r\n");
		break;
	}
	return;
}

/*void gatherNeighbours(){ //REDUNDANT
//calculate number of elements in neighbour table
	//int 	NumNeighbours = (sizeof(neighbours)/sizeof(neighbours[0]));
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
}*/