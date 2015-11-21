#define MAXMESSAGELENGTH 30
#define MAXSEGMENTLENGTH 121

#include <string.h>

uint16_t calcrc(char *ptr, int count);
void display_segment(char* segment);
void ctrl_read(uint8_t* encrypted, uint8_t* flag1, uint8_t* flag2,
        uint8_t* segmentnumber, uint8_t* segmenttotal, char* ptr);
void ctrl_write(uint8_t encrypted, uint8_t flag1, uint8_t flag2,
        uint8_t segmentnumber, uint8_t segmenttotal, char* ptr);
void copyin(char* dest, char* source, );

int SendData(char dest, char* sdata)
{
    uint8_t encrypted, flag1, flag2;
    uint8_t segmentnumber, segmenttotal;

	char segment[MAXSEGMENTLENGTH+1] = {0}; //Initialisation is important

    ctrl_write(0, 0, 0, 1, 1, segment);
    segment[2] = 0x00; //Source port
    segment[3] = 0x00; //Source port
    segment[4] = 30;

    copyin(segment, sdata, 30);

    uint16_t crcbits = calcrc(segment, 35);

    display_segment(segment);

	uint16_t crcbits = calcrc(segment, 120);

    ctrl_read(&encrypted, &flag1, &flag2, &segmentnumber, &segmenttotal, &segment[0]);

	int error = SendSegment(dest, segment);
	
	return 0;
};

// int RecieveData(char source, char* rdata)
// {

// }

//To check values use http://www.lammertbies.nl/comm/info/crc-calculation.html
uint16_t calcrc(char *ptr, int count) //XModem CRC calculator from https://github.com/vinmenn/Crc16
{
    int  crc;
    char i;
    crc = 0;
    while (--count >= 0)
    {
        crc = crc ^ (int) *ptr++ << 8;
        i = 8;
        do
        {
            if (crc & 0x8000)
                crc = crc << 1 ^ 0x1021;
            else
                crc = crc << 1;
        } while(--i);
    }
    return (crc);
}

void display_segment(char* segment)
{
    uint8_t i = 0;
    uint8_t encrypted, flag1, flag2;
    uint8_t segmentnumber, segmenttotal;
    uint16_t crc = 0;

    ctrl_read(&encrypted, &flag1, &flag2, &segmentnumber, &segmenttotal, segment);

    display_string("Decoding segment that is ");
    display_number(strlen(segment));
    display_string(" bytes long.\n\n");

    display_string("Control byte 1: ");
    display_binary(segment[0]);
    display_string("\nControl byte 2: ");
    display_binary(segment[1]);

    display_string("\nSegment: ");
    display_number(segmentnumber);
    display_string(" out of ");
    display_number(segmenttotal);

    display_string("\nSource port: ");
    display_hex(segment[2], 1);
    display_string("\nDest port: ");
    display_hex(segment[3], 1);

    display_string("\nLength: ");
    display_number(segment[4]);

    display_string("\nMessage: ");
    for (i = 5; i<(segment[4]+5); i++)
        display_char(segment[i]);

    display_string("\nCRC: ");
    crc = (segment[i] << 8);
    crc |= (segment[i+1]);
    display_hex(crc, 2);
}

void ctrl_read(uint8_t* encrypted, uint8_t* flag1, uint8_t* flag2,
        uint8_t* segmentnumber, uint8_t* segmenttotal, char* ptr) //Reads a segment's control bits
{
    //if (ptr[0] & (0x01 << 7)) //Needs to be one to be a segment but we can ignore
    //Definitely a better way to do the below, but didn't work unless I do it longhand
    if(ptr[0] & (1 << 6))
        *encrypted = 1;
    else
        *encrypted = 0;
    if(ptr[0] & (1 << 5))
        *flag1 = 1;
    else
        *flag1 = 0;
    if(ptr[0] & (1 << 4))
        *flag2 = 1;
    else
        *flag2 = 0;

    *segmentnumber = 0;
    for (int i = 3; i>=0; i--)
        *segmentnumber |= ((ptr[0] & (1 << i)) << 2);
    *segmentnumber |= ((ptr[1] & (1 << 7)) >> 6);
    *segmentnumber |= ((ptr[1] & (1 << 6)) >> 6);

    *segmenttotal = (ptr[1] & 0x3f);
}

void ctrl_write(uint8_t encrypted, uint8_t flag1, uint8_t flag2,
        uint8_t segmentnumber, uint8_t segmenttotal, char* ptr) //Reads a segment's control bits
{
    //if (ptr[0] & (0x01 << 7)) //Needs to be one to be a segment but we can ignore
    //Definitely a better way to do the below, but didn't work unless I do it longhand

    ptr[0] = 0;
    ptr[1] = 0;

    ptr[0] |= 1 << 7; //Sets first bit to one to say it's a segment
    ptr[0] |= encrypted << 6;
    ptr[0] |= flag1 << 5;
    ptr[0] |= flag2 << 4;

    ptr[0] |= segmentnumber >> 2;
    ptr[1] |= (segmentnumber & 0x03) << 6;
    ptr[1] |= segmenttotal & 0x3f;
}