#include <string.h>

uint16_t calcrc(char *ptr, int count);

int SendData(char dest, char* sdata)
{
	char segment1[120] = {0};
	char crc[3] = {0};
	strncpy (segment1, sdata, 50);

	uint16_t crcbits = calcrc(segment1, 50);

	crc[0] = (char)((crcbits & 0xFF00) >> 8);
	crc[1] = (char)(crcbits & 0x00FF);

	SendSegment(dest, segment1);

	display_string("CRC: ");
	display_string(crc);

	display_string(" and in hex: ");

	char crchex[4];

	if (crcbits <= 0xFFFF) //Converts CRC to hex array for debugging
	{
	    sprintf(&crchex[0], "%04x", crcbits);
	}

	display_string(crchex);
	
	return 0;
}

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

//This function converts to hex
// char crchex[4];

// if (crc <= 0xFFFF) //Converts CRC to hex array for debugging
// {
//     sprintf(&crchex[0], "%04x", crc);
// }