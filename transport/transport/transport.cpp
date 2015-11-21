#include <string.h>

uint16_t calcrc(char *ptr, int count);
void display_segment(char dest, char* segment);

int SendData(char dest, char* sdata)
{
	char segment1[121]; 
	strncpy (segment1, sdata, 120);

	uint16_t crcbits = calcrc(segment1, 120);
	int error = SendSegment(dest, segment1);
	display_segment(dest, segment1);
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

void display_segment(char dest, char* segment)
{
    uint8_t i = 0;

    display_string("Sending segment to ");
    display_char(dest);
    display_string(" that is ");
    display_number(strlen(segment));
    display_string(" bytes long.\n");
}