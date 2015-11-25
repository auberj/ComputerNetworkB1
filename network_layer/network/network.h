//network.h
// https://docs.google.com/document/d/1VcHl5GLs-VoUbCynElk380cPVeAX4LO5zO68r2fjCKA/edit

#include "network_definitions.h"
#include "../link_dummy/link.h"

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