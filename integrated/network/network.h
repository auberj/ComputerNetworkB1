//network.h
// https://docs.google.com/document/d/1VcHl5GLs-VoUbCynElk380cPVeAX4LO5zO68r2fjCKA/edit

//#include "../link/link.cpp"

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