#include <string.h>

int SendSegment(char dest, char* ssegment)
{
	display_string("Sending segment to ");
	display_char(dest);
	display_string(" with message: ");
	display_string(ssegment);
	return 0;
}

// int RecieveSegment(char source, char* rsegment)
// {

// }