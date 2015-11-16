
#ifndef __DATALINK_H__
#define __DATALINK_H__

//Size of each section of the frame
//#define HEADERLEN 1
#define CONTROLLEN 2
#define ADDRESSLEN 2
//#define LENGTHLEN 1
#define DATALEN 23
#define CHECKSUMLEN 2
//#define FOOTERLEN 1

//number of max number of frames to be sent
#define PACKETLEN 122
#define FRAMECOUNT ((PACKETLEN/DATALEN) + 1)

//header, footer, break values
#define HEADER '\x32'
#define FOOTER '\x32'

//default addresses
 



struct frame {
	char header;
	char control[CONTROLLEN];
	char address[ADDRESSLEN];
	char length;
	char data[DATALEN];
	char checksum[CHECKSUMLEN];
	char footer;
};

void setdata(struct frame (*vals)[FRAMECOUNT], char* Spacket);
void setfooter(struct frame (*vals)[FRAMECOUNT]);
void setlength(struct frame (*vals)[FRAMECOUNT]);
void setheader(struct frame (*vals)[FRAMECOUNT]);
void setlength(struct frame (*vals)[FRAMECOUNT]); 
void setaddress(struct frame (*vals)[FRAMECOUNT], char* address);

int SendPacket(char dest, char* Spacket);
int RecievePacket(char* Rpacket);

#endif