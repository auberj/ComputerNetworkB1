#include "uart1/uart1.c"

int SendPacket(char dest, char* spacket)
{
	put_1_string(spacket);
	put_1_char('\0');
	return 0;
}

int RecievePacket(char* rpacket)
{
	char temp = 'A';
	int i = 0;
	while(temp != 0)
	{
		temp = get_1_char();
		if (temp != 0)
		{
			rpacket[i] = temp;
			put_char(rpacket[i]);
			i++;
		}
	}
	if (strlen(rpacket))
	{
		put_string("\r\nReceived packet\r\n");
		return 1;
	}
	return 0;
}