//network.h


#include "network_definitions.h"
#include "../link_dummy/link.h"

void 	gatherNeighbours(char* neighbourtable);
void 	sendHello();
int 	getPacket(char* neighbourADD);