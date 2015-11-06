//physical
//#include "physical.h"

int SendPacket(char dest, char* packet){
	display_char(packet[8]);
	return 0;
}

int RecievePacket(char* packet){
	char dummy_packet[128];

	dummy_packet[0] = 0xFF;
	dummy_packet[1] = 0xFF;
	//set SRC address
	dummy_packet[2] = 0x02;
	//set DEST address (doesn't matter for HELLO)
	dummy_packet[3] = 0x01;
	//set length
	dummy_packet[4] = MaxSegmentLength;
	//set segment to be empty
	for(int i=5;i<125;i++){
		dummy_packet[i] = 'i';
	}
	dummy_packet[126] = 0x00;
	dummy_packet[127] = 0x00;
	return 0;
}