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
//Control bits: 
//
//
//
//
//
//
//
//
//
//
//
//
//
///////////////////////////
