#include <string.h>

int SendSegment(char dest, char* ssegment)
{
	display_string("Sending segment to ");
	display_char(dest);
	display_string(" with message: ");
	display_nstring(ssegment, 0, 120);
	display_char('\n');
	return 0;
}

// int RecieveSegment(char source, char* rsegment)
// {

// }