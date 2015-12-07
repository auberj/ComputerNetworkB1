//network.h
// https://docs.google.com/document/d/1VcHl5GLs-VoUbCynElk380cPVeAX4LO5zO68r2fjCKA/edit

//#include "../link/link.cpp"

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


void 	gatherNeighbours();
void 	sendHello();
int 	getPacket(char* neighbourADD);
int 	SendSegment(char dest, char* segment);
int 	RecieveSegment(char* source, char* rsegment);
void 	getNeighbourAdd(char* neighbourADD, char* packet); //extracts neighbour address from packet
void 	sendNeighbours();
void 	processHello(char* packet); //processes a HELLO message detected in a packet
uint16_t calcrc(char *ptr, int count);
int		checkRepeatPacket(char* packet); //return 1 if checksum is currently in the checksum table
void	displaySegment(char* packet);
int		checkRecievedPacket(char* packet); //check if a flooded packet has already been received
void	displayPacket(char* packet,int command);
char 	calcNextHop(char dest);
void 	periodicHello();
void 	processNeighbours(char* packet);
int 	isANeighbour(char* address);