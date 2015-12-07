#include "uart1/uart1.c"

int SendPacket(char dest, char* spacket)
{
	init_uart1();
	put_1_string(spacket);
	return 0;
}

int RecievePacket(char* rpacket)
{
	char temp = 'A';
	int i = 0;

	while(temp != 0)
	{
		temp = get_1_char()
		if (temp != 0)
		{
			rpacket[i] = temp;
			i++;
		}
	}
	return 0;
}