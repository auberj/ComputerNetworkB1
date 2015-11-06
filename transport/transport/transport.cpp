#include <string.h>

void senddata(char* message)
{
	char segment1[120];
	strncpy (segment1, message, 100);
	segment1[100] = '\0';
	display_string(segment1);
}