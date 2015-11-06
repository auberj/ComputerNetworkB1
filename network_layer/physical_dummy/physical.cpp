//physical
//#include "physical.h"

int SendPacket(char dest, char* packet){
	display_char(packet[8]);
	return 0;
}

int RecievePacket(char* packet){

	//
	packet[0] = 'k';
	packet[1] = 'X';
	//set SRC address
	packet[2] = 'g';
	//set DEST address (doesn't matter for HELLO)
	packet[3] = 0x01;
	//set length
	packet[4] = MaxSegmentLength;
	//set segment to be empty
	for(int i=5;i<125;i++){
		packet[i] = 'i';
	}
	packet[126] = 0xFF;
	packet[127] = 0xFF;
	return 0;
}