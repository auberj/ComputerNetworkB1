#include "uart1/uart1.c"

int SendSegment(char dest, char* spacket)
{
	UART_flush();
	put_string("\r\nBegin: SendSegment\r\n");
	put_debug_string(spacket);
	put_1_string(spacket);
	put_1_char('\0');
	put_string("\r\nEnd: SendSegment\r\n");
	//UART_flush();
	return 0;
}

int RecieveSegment(char* source, char* rpacket)
{
	//UART_flush();
	put_string("\r\nBegin: RecieveSegment\r\n");
	char temp = 'A';
	int i = 0;
	while(temp != 0)
	{
		//put_string("#");
		temp = get_1_char();
		if (temp !=  0)
		{
			//put_string(".");
			rpacket[i] = temp;
			//put_char(rpacket[i]);
			i++;
		}
		//put_string("@");
	}
	//rpacket[i] = '\0';
	if (strlen(rpacket))
	{
		*source = rpacket[2];
		put_string("\r\nReceived Segment\r\n");
		put_string("\r\nEnd: RecieveSegment 1\r\n");
		//UART_flush();
		return 1;
	}
	else{
		put_string("\r\nEnd: RecieveSegment 0\r\n");
		//UART_flush();
		return 0;
	}

		
}