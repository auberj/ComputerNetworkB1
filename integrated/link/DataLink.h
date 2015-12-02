
#ifndef __DATALINK_H__
#define __DATALINK_H__

#include "lcd/font.c"
#include "lcd/ili934x.c"
#include "lcd/lcd.c"
#include "lcd/avrlcd.h"

#include "rfm12.h"

//Size of each section of the frame
#define HEADERLEN 1
#define CONTROLLEN 2
#define ADDRESSLEN 2
#define LENGTHLEN 1
#define DATALEN 10
#define CHECKSUMLEN 2
#define FOOTERLEN 1
#define FRAMELEN (HEADERLEN + CONTROLLEN + ADDRESSLEN + LENGTHLEN + DATALEN + CHECKSUMLEN + FOOTERLEN + 10)

//number of max number of frames to be sent
#define PACKETLEN 128
#define FRAMECOUNT ((PACKETLEN/DATALEN) + 1)

//header, footer, break values
// #define HEADER '\x1a'
// #define FOOTER '\x1a'
// #define ESCAPE '\x10'
// #define START '\x02'
// #define END '\x03'
#define HEADER '\x73'
#define FOOTER '\x72'
#define ESCAPE '\x61'
#define START '\x35'
#define END '\x35'
//default addresses
 
//control bits
#define INFOFRAME "IF"
#define SUPEFRAME "SF"

//built in addresses
#define BROADCAST '\xff'
#define THISDEVICE 'N' //0x74

//CRC stuff
#define GENERATOR 0x1021


struct frame {
	char header[HEADERLEN+1];
	char control[CONTROLLEN+1];
	char address[ADDRESSLEN+1];
	char length[LENGTHLEN+1];
	char data[DATALEN+1];
	char checksum[CHECKSUMLEN+1];
	char footer[FOOTERLEN+1];
	char frame[FRAMELEN + 1];
};

void unbytestuff(char *Rframe);
int decode_frame(struct frame *framedata, char * Rframe);

int makeframe(struct frame (*data)[FRAMECOUNT], char dest, char*Spacket, int ack, int frames);
int bytestuff(char *str, int len);
uint16_t calccrc(char *str, int len);
void setdata(struct frame (*vals)[FRAMECOUNT], char* Spacket, int ack);
void setfooter(struct frame (*vals)[FRAMECOUNT]);
void dataInit(struct frame (*vals)[FRAMECOUNT]);
void setheader(struct frame (*vals)[FRAMECOUNT]);
void setaddress(struct frame (*vals)[FRAMECOUNT], char* address);
void setcontrol(struct frame (*vals)[FRAMECOUNT], int ack);

int SendPacket(char dest, char* Spacket);
int RecievePacket(char* Rpacket);

#endif