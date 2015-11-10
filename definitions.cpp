/*These are the services that each layer provides
to the layer above it
*/

//Transport (DOM)

int SendData(char dest, char* sdata);
int RecieveData(char source, char* rdata);

//Network (NATHAN)

int SendSegment(char dest, char* segment); //return 0
int RecieveSegment(char* source, char* rsegment); //return 0 if no segment available, return 1 if segment extration is succesful

//Link (HUW)

int SendPacket(char dest, char* Spacket);
int RecievePacket(char* rpacket); //the recieved packet will be put in too Rpacket

//Physical

int SendFrame(destination, &frame);
int RecieveFrame(&source, &frame);

////////Transport/////////

// Control bits
// 	First (MSB) bit is always a 1. Next is a 1 if encrpyted
// Next two bits are for extra functionaility
// Next 6 say which segment it is
// Next 6 say which segment it's out of

// To say this is the 1st segment out of 5
// SRC port = 0x00

// Dest port = 0x00

// Length
// 	Length, a uint8_t which says length of appdata in the packet. Eg if message is only 50 characters, length will be 50

// App data
// 	Up to 114 char array of application data. Anymore than 114 will be split into two packets

// Checksum
// 	We have 16 bits for a checksum. We will use the CRC16 XModem checksum. This is very good and has a fixed length. The 16 bits are to be split into two uint8_ts and then converted into two characters. The MSB 8 bits will go in the first.
// 	http://www.lammertbies.nl/comm/info/crc-calculation.html

///////////////////////////



/*---NETWORK---//

Control Bits: (X = dont care)
	HX: Hello