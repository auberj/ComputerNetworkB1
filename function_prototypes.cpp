/*These are the services that each layer provides
to the layer above it
*/

//Transport (DOM)

SendData(destination, &data);
RecieveData(&source, &data);

//Network (NATHAN)

SendSegment(destination, &segment);
RecieveSegment(&source, &segment);

//Link (HUW)

int SendPacket(char dest, char* packet);
int RecievePacket(char source, char* packet);

//Physical

SendFrame(destination, &frame);
RecieveFrame(&source, &frame);

