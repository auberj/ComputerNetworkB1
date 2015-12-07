#include "uart1/uart1.c"

int SendPacket(char dest, char* spacket)
{
	put_string("\r\nBegin: SendPacket\r\n");
	put_1_string(spacket);
	put_1_char('\0');
	put_string("\r\nEnd: SendPacket\r\n");
	return 0;
}

int RecievePacket(char* rpacket)
{
	put_string("\r\nBegin: RecievePacket\r\n");
	char temp = 'A';
	int i = 0;
	while(temp != 0)
	{
		//put_string("#");
		temp = get_1_char();
		if (temp != 0)
		{
			//put_string(".");
			rpacket[i] = temp;
			//put_char(rpacket[i]);
			i++;
		}
		//put_string("@");
	}
	if (strlen(rpacket))
	{
		put_string("\r\nReceived packet\r\n");
		put_string("\r\nEnd: RecievePacket 1\r\n");
		return 1;
	}
	else{
		put_string("\r\nEnd: RecievePacket 0\r\n");
		return 0;
	}

		
}