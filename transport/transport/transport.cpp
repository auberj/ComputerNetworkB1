#include <string.h>

void senddata(char* message)
{
	char segment1[120];
	strncpy (segment1, message, 10);
	segment1[10] = '\0';
	display_string(segment1);
}