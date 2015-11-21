#include <string.h>

uint16_t calcrc(char *ptr, int count);

int SendData(char dest, char* sdata)
{
	char segment1[121];
	strncpy (segment1, sdata, 120);

	uint16_t crcbits = calcrc(segment1, 120);
	int error = SendSegment(dest, segment1);

	display_string("CRC in hex: ");
	
	display_hex(crcbits);
	return 1;
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

//This function converts to hex
// char crchex[4];

// if (crc <= 0xFFFF) //Converts CRC to hex array for debugging
// {
//     sprintf(&crchex[0], "%04x", crc);
// }