#include <stdio.h>

#include "link.h"

#define MAXPACKETSIZE 128

//link
int simulation_type;
char DummyPacket[MAXPACKETSIZE+1];
int callcount;

void set_simulation_type(int sim){
	simulation_type = sim;
	callcount = 0;
	return;
}

int SendPacket(char dest, char* packet){
	//switch(simulation_type){
	//	case 0:
			callcount = 0;
			put_string("BEGIN SEND PACKET\r\n");
			int PacketLength = strlen(packet);
			for(int i=0;i<(MAXPACKETSIZE+1);i++){
				DummyPacket[i]=0;
			}
			
			//display_string("s packet length: "); display_number(PacketLength); display_string("\n");
			for(int i=0;i<(PacketLength);i++){
				DummyPacket[i]=packet[i];
			}
			put_string("sending packet: ");
			for(int i=0;i<(PacketLength);i++){
			 	put_char(DummyPacket[i]);
			 	
			 }
			//strcpy(DummyPacket,packet);
			put_string("\r\nEND SEND PACKET\r\n");
			return 0;
	//	break;
	//}
}

int RecievePacket(char* packet){
	int returnval;
	int PacketLength;
	if(callcount==0){
		switch(simulation_type){
		case 0:
			put_string("BEGIN REC PACKET - 0\r\n");
			//strcpy(packet,DummyPacket);
			PacketLength = strlen(DummyPacket);
			put_string("Dummy packet length: "); put_number(PacketLength); put_string("\r\n");
			//display_string("r packet length: "); display_number(PacketLength); display_string("\n");
			for(int i=0;i<(PacketLength);i++){
				packet[i] = DummyPacket[i];
			}
			
			PacketLength = strlen(packet);
			put_string("Real packet length: "); put_number(PacketLength); put_string("\r\n");
			put_string("END REC PACKET\r\n");
			callcount = 1;
			returnval = 1;
			for(int i=0;i<(MAXPACKETSIZE+1);i++){
				DummyPacket[i] = 0;
			}
		break;	
		case 1:
			put_string("BEGIN REC PACKET - 1\r\n");
			//strcpy(packet,DummyPacket);
			PacketLength = strlen(DummyPacket);
			put_string("Dummy packet length: "); put_number(PacketLength); put_string("\r\n");
			//display_string("r packet length: "); display_number(PacketLength); display_string("\n");
			for(int i=0;i<(PacketLength);i++){
				packet[i] = DummyPacket[i];
			}
			
			PacketLength = strlen(packet);
			put_string("Real packet length: "); put_number(PacketLength); put_string("\r\n");
			put_string("END REC PACKET\r\n");
			callcount = 1;
			returnval = 1;
			for(int i=0;i<(MAXPACKETSIZE+1);i++){
				DummyPacket[i] = 0;
			}
		break;	
		case 2:
			put_string("BEGIN REC PACKET - 2\r\n");
			//strcpy(packet,DummyPacket);
			PacketLength = strlen(DummyPacket);
			put_string("Dummy packet length: "); put_number(PacketLength); put_string("\r\n");
			//display_string("r packet length: "); display_number(PacketLength); display_string("\n");
			for(int i=0;i<(PacketLength);i++){
				packet[i] = DummyPacket[i];
			}
			
			PacketLength = strlen(packet);
			put_string("Real packet length: "); put_number(PacketLength); put_string("\r\n");
			put_string("END REC PACKET\r\n");
			callcount = 0;
			returnval = 1;
		break;	
		case 3:
			put_string("BEGIN REC PACKET - 1\r\n");
			//strcpy(packet,DummyPacket);
			PacketLength = strlen(DummyPacket);
			put_string("Dummy packet length: "); put_number(PacketLength); put_string("\r\n");
			//display_string("r packet length: "); display_number(PacketLength); display_string("\n");
			for(int i=0;i<(PacketLength);i++){
				packet[i] = DummyPacket[i];
			}
			
			PacketLength = strlen(packet);
			put_string("Real packet length: "); put_number(PacketLength); put_string("\r\n");
			put_string("END REC PACKET\r\n");
			callcount = 1;
			returnval = 1;
			for(int i=0;i<(MAXPACKETSIZE+1);i++){
				DummyPacket[i] = 0;
			}
		break;
		case 4:
			put_string("BEGIN REC PACKET - 0\r\n");
			//strcpy(packet,DummyPacket);
			PacketLength = strlen(DummyPacket);
			put_string("Dummy packet length: "); put_number(PacketLength); put_string("\r\n");
			//display_string("r packet length: "); display_number(PacketLength); display_string("\n");
			for(int i=0;i<(PacketLength);i++){
				packet[i] = DummyPacket[i];
			}
			
			PacketLength = strlen(packet);
			put_string("Real packet length: "); put_number(PacketLength); put_string("\r\n");
			put_string("END REC PACKET\r\n");
			callcount = 1;
			returnval = 1;
			for(int i=0;i<(MAXPACKETSIZE+1);i++){
				DummyPacket[i] = 0;
			}
		break;	

		case 5:
			put_string("BEGIN REC PACKET - 0\r\n");
			//strcpy(packet,DummyPacket);
			PacketLength = strlen(DummyPacket);
			put_string("Dummy packet length: "); put_number(PacketLength); put_string("\r\n");
			//display_string("r packet length: "); display_number(PacketLength); display_string("\n");
			for(int i=0;i<(PacketLength);i++){
				packet[i] = DummyPacket[i];
			}
			
			PacketLength = strlen(packet);
			put_string("Real packet length: "); put_number(PacketLength); put_string("\r\n");
			put_string("END REC PACKET\r\n");
			callcount = 1;
			returnval = 1;
			for(int i=0;i<(MAXPACKETSIZE+1);i++){
				DummyPacket[i] = 0;
			}
		break;	
		}
	}
	else{
		returnval = 0;
	}
	
	return returnval;
}