#define MAXSEGMENTLENGTH 27
#define MAXSEGMENTS 5

char globalsegmentnumber;
char globalpacket[MAXSEGMENTLENGTH+1][MAXSEGMENTS];
char globaldest;

int SendSegment(char dest, char* spacket);
int RecieveSegment(char* source, char* rpacket);

int SendSegment(char dest, char* spacket) //TODO use better acks. - don't store if flag 1 is set
{
	int i;
	for (i = 0; spacket[i]; i++)
		globalpacket[i][globalsegmentnumber] = spacket[i];
	globalpacket[i][globalsegmentnumber] = ('\0');
	globaldest = dest;
	put_string("Sending segment");
	return 0;
}

int RecieveSegment(char* source, char* rpacket) //return the checksum segment on alternate calls (have a state, like the other device will)
{
	if (millis() > 3000)
	{
		//if flag 1 is set, turn it into a checksum acknowledgment
		for (int i = 0; globalpacket[i][globalsegmentnumber]; i++)
			rpacket[i] = globalpacket[i][globalsegmentnumber];
		*source = 'H';
		put_string("Receiving segment");
		return 1;
	}
	else
		return 0;
}