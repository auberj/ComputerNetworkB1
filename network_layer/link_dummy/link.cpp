//link

int SendPacket(char dest, char* packet){
	display_string("packet passed to link layer\n");
	return 0;
}

int RecievePacket(char* packet){

	//
	packet[0] = 'H'; //H for hello
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