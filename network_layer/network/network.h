//network.h


#include "network_definitions.h"
#include "../link_dummy/link.h"

void 	gatherNeighbours(char* neighbourtable);
void 	sendHello();
int 	getPacket(char* neighbourADD);
int 	SendSegment(char dest, char* segment);
int 	RecieveSegment(char source, char* rsegment);
void 	getNeighbourAdd(char* neighbourADD, char* packet); //extracts neighbour address from packet
void 	sendNeighbours(char* neighbourtable);