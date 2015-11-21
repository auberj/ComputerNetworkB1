
#ifndef __DATALINK_H__
#define __DATALINK_H__

//Size of each section of the frame
#define HEADERLEN 1
#define CONTROLLEN 2
#define ADDRESSLEN 2
#define LENGTHLEN 1
#define DATALEN 23
#define CHECKSUMLEN 2
#define FOOTERLEN 1

//number of max number of frames to be sent
#define PACKETLEN 122
#define FRAMECOUNT ((PACKETLEN/DATALEN) + 1)

//header, footer, break values
#define HEADER '\x1a'
#define FOOTER '\x1a'
#define ESCAPE '\x10'

//default addresses
 
//control bits
#define INFOFRAME "IF"
#define SUPEFRAME "SF"

//CRC stuff
#define GENERATOR 0x1021



struct frame {
	char header;
	char control[CONTROLLEN];
	char address[ADDRESSLEN];
	char length;
	char data[DATALEN];
	char checksum[CHECKSUMLEN];
	char footer;
};

int bytestuff(char *str, int len);
uint16_t calccrc(char *str, int len);
void setdata(struct frame (*vals)[FRAMECOUNT], char* Spacket);
void setfooter(struct frame (*vals)[FRAMECOUNT]);
void setlength(struct frame (*vals)[FRAMECOUNT]);
void setheader(struct frame (*vals)[FRAMECOUNT]);
void setlength(struct frame (*vals)[FRAMECOUNT]); 
void setaddress(struct frame (*vals)[FRAMECOUNT], char* address);
void setcontrol(struct frame (*vals)[FRAMECOUNT]);

int SendPacket(char dest, char* Spacket);
int RecievePacket(char* Rpacket);

#endif