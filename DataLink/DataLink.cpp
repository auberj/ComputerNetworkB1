/*
	The data link layer

	receives a packet from the network layer and provides a reliable link to the next transceiver, ensuring the
	data arrives in the order it was sent. Acts like a wire.

	Frame Structure:

	| Header[1] | control[2] | Addressing[2] | length[1] | Data[1-23] | Checksum[2] | Foter[1] |

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
	/*
	split packet into frames, send frame, await acknowledgement  
	*/

}
int RecievePacket(char* Rpacket) {
	/*
	receive frame, send acknowledgement
	*/

}