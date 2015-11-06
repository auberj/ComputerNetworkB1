/*
	The data link layer

	receives a packet from the network layer and provides a reliable link to the next transceiver, ensuring the
	data arrives in the order it was sent. Acts like a wire.

	Features:
		Acknowledged connectionless link.
			- probable look into connection oriented at a later date

		Framing Method - bit stuffing (have to check what is currently being implemented in the rfm library)

		Flow control - probably not needed

		error control - maybe

*/

#include <iostream>
using namespace std;


int SendPacket(char dest, char* Spacket) {

}
int RecievePacket(char* Rpacket) {

}