#include <string.h>

int SendData(char dest, char* sdata)
{
	char segment1[120];
	
	strncpy (segment1, sdata, 100);
	segment1[100] = '\0';


	SendSegment(dest, segment1);
	return 0;
}

// int RecieveData(char source, char* rdata)
// {

// }