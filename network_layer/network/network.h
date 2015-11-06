//network.h


#include "network_definitions.h"
#include "../physical_dummy/physical.h"

void 	gatherNeighbours(char* neighbourtable);
void 	sendHello();
int 	getPacket(char* neighbourADD);