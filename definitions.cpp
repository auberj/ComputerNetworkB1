/*These are the services that each layer provides
to the layer above it
*/

//Transport (DOM)

int SendData(char dest, char* sdata);
int RecieveData(char source, char* rdata);

//Network (NATHAN)

int SendSegment(char dest, char* ssegment);
int RecieveSegment(char source, char* rsegment);

//Link (HUW)

int SendPacket(char dest, char* spacket);
int RecievePacket(char source, char* rpacket);

//Physical

int SendFrame(destination, &frame);
int RecieveFrame(&source, &frame);

////////Transport/////////
Control bits
	SE: Segment
	SX: Segment (encrypted)

To say this is the 1st segment out of 5
SRC port
	Char which is which segment this is eg 1

Dest port
	Char which is the number of total segments in message eg 5

Length
	Length, a char which says length of data in the packet. Eg if message is only 50 characters, length will be 50

App data
	Up to 114 char array of application data. Anymore than 114 will be split into two packets

Checksum


///////////////////////////
