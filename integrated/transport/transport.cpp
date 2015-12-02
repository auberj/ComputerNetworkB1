#define MAXMESSAGELENGTH 20
#define MAXSEGMENTS 10 //This times the above number shouldn't be less than chars in message
#define TIMEOUTMILLIS 1000 //milliseconds for timeout

int SendData(char dest, char* sdata);
int RecieveData(char* source, char* rdata, uint8_t messageflag);
//uint16_t calcrc(char *ptr, int count);
void display_segment(char* segment);
void ctrl_read(uint8_t* encrypted, uint8_t* flag1, uint8_t* flag2,
        uint8_t* segmentnumber, uint8_t* segmenttotal, char* ptr);
void ctrl_write(uint8_t encrypted, uint8_t flag1, uint8_t flag2,
        uint8_t segmentnumber, uint8_t segmenttotal, char* ptr);
void copyin(char* dest, char* source, uint8_t start, uint8_t number, uint16_t srcptr);
uint8_t waitacknowledge(char dest, char* segment);

int SendData(char dest, char* sdata)
{
    volatile uint8_t loop = 0;
    uint8_t i = 1;
    uint8_t j;
    uint8_t encrypted, flag1, flag2;
    uint8_t segmentnumber, segmenttotal;
    uint8_t messagelength;
    uint16_t crcbits;
    uint16_t sdatalength;
	char segment[MAXSEGMENTS][MAXMESSAGELENGTH+8] = {'\0'}; //Initialisation is important

    sdatalength = strlen(sdata);

    uint8_t numberofsegments = (uint8_t)(sdatalength/MAXMESSAGELENGTH);
    if (numberofsegments*MAXMESSAGELENGTH < sdatalength) 
        numberofsegments++;

    while (loop < numberofsegments)
    {
        i = 1;
        for (j = 0; j < MAXMESSAGELENGTH+8; j++)
            segment[loop][j] = 0;

        //TODO write encryption

        ctrl_write(0, 0, 0, loop+1, numberofsegments, segment[loop]);
        segment[loop][2] = 0xFF; //Source port
        segment[loop][3] = 0xFF; //Dest port

        if (loop != numberofsegments - 1)
            messagelength = MAXMESSAGELENGTH;
        else
            messagelength = sdatalength - (loop*MAXMESSAGELENGTH);

        segment[loop][4] = messagelength;
        copyin(segment[loop], sdata, 5, messagelength, (loop*MAXMESSAGELENGTH));
        crcbits = calcrc(segment[loop], messagelength+5);

        segment[loop][messagelength+5] = crcbits >> 8;
        segment[loop][messagelength+6] = crcbits & 0x00FF;

        display_segment(segment[loop]);
        put_string("\r\n");

        while(i)
        {
            put_string("\r\n\r\nSending segment to ");
            put_char(dest);
            SendSegment(dest, segment[loop]);
            put_string("\r\nSegment sent, waiting on acknowledgment\r\n");
            i = waitacknowledge(dest, segment[loop]); //returns 1 if needs to go round the loop again
            if(i)
                put_string("\r\nNo acknowledgment, resending segment");

        }
        
        put_string("\r\n\r\nAcknowledged.");
        _delay_ms(1);
        loop = loop + 1;
    }
    //ctrl_read(&encrypted, &flag1, &flag2, &segmentnumber, &segmenttotal, &segment[0]);

	//int error = SendSegment(dest, segment);
	
	return 0;
};

int RecieveData(char* source, char* rdata, uint8_t* rmessageflag)
{
    int receiveflag = 0;
    char segment[MAXMESSAGELENGTH+8] = {'\0'};
    uint8_t encrypted, flag1, flag2;
    uint8_t segmentnumber, segmenttotal;
    uint16_t crc = 0;

    receiveflag = RecieveSegment(source, segment);

    // if (millis() > 2000); //This is dummy received data representing Hello World!
    // {
    //     segment[0] = 0b10000000;
    //     segment[1] = 0b01000001;
    //     segment[2] = 0b11111111;
    //     segment[3] = 0b11111111;
    //     segment[4] = 0b00001100;
    //     segment[5] = 0b01001000;
    //     segment[6] = 0b01100101;
    //     segment[7] = 0b01101100;
    //     segment[8] = 0b01101100;
    //     segment[9] = 0b01101111;
    //     segment[10] = 0b00100000;
    //     segment[11] = 0b01010111;
    //     segment[12] = 0b01101111;
    //     segment[13] = 0b01110010;
    //     segment[14] = 0b01101100;
    //     segment[15] = 0b01100100;
    //     segment[16] = 0b00100001;
    //     segment[17] = 0b10011000;
    //     segment[18] = 0b11100011;
    //     *source = 'D';
    //     receiveflag = 1;
    // }

    if (receiveflag) //if something has been received
    {
        uint8_t segmentlength = strlen(segment);
        ctrl_read(&encrypted, &flag1, &flag2, &segmentnumber, &segmenttotal, segment);

        put_string("\r\n");
        put_number(strlen(segment));
        put_string(" byte long segment from ");
        put_char(*source);
        put_string(" received.");
        display_segment(segment);
        put_string("\r\n");

        SendSegment(*source, segment); //Acknowledge the segment

        //TODO read flags properly eg encryption
        //TODO decryption
        //TODO check segment valid

        //todo receive flag = 2 if message incomplete
        copyin(rdata, segment, 0, segmentlength-7, 5);
        *rmessageflag = segmenttotal - segmentnumber;
    }
    

    return receiveflag;
}

void display_segment(char* segment)
{
    uint8_t i = 0;
    uint8_t encrypted, flag1, flag2;
    uint8_t segmentnumber, segmenttotal;
    uint16_t crc = 0;

    ctrl_read(&encrypted, &flag1, &flag2, &segmentnumber, &segmenttotal, segment);

    put_string("\r\n\r\nDecoding segment that is ");
    put_number(strlen(segment));
    put_string(" bytes long.");

    put_string("\r\n\r\nControl Byte 1: ");
    put_binary(segment[0]);
    put_string("\r\nControl Byte 2: ");
    put_binary(segment[1]);

    put_string("\r\nSegment: ");
    put_number(segmentnumber);
    put_string(" of ");
    put_number(segmenttotal);

    put_string("\r\nSource Port: ");
    put_hex(segment[2], 1);
    put_string("\r\nDest Port: ");
    put_hex(segment[3], 1);

    put_string("\r\nMessage Length: ");
    put_number((uint8_t)segment[4]);

    put_string("\r\nMessage: ");
    for (i = 0; i<(strlen(segment)-7); i++)
        put_char(segment[i+5]);

    put_string("\r\nCRC: ");;
    crc = (segment[strlen(segment) - 2] << 8);
    crc |= (segment[strlen(segment) - 1] & 0x00ff);
    put_hex(crc, 2);

    put_string("\r\nSegment Valid: ");
    //put_hex(calcrc(segment, (strlen(segment)-2)), 2);
    if (crc == calcrc(segment, (strlen(segment)-2)))
        put_string("Yes");
    else
        put_string("No");
}

void ctrl_read(uint8_t* encrypted, uint8_t* flag1, uint8_t* flag2,
        uint8_t* segmentnumber, uint8_t* segmenttotal, char* ptr) //Reads a segment's control bits
{
    //if (ptr[0] & (0x01 << 7)) //Needs to be one to be a segment but we can ignore
    //Definitely a better way to do the below, but didn't work unless I do it longhand
    if(ptr[0] & (1 << 6))
        *encrypted = 1;
    else
        *encrypted = 0;
    if(ptr[0] & (1 << 5))
        *flag1 = 1;
    else
        *flag1 = 0;
    if(ptr[0] & (1 << 4))
        *flag2 = 1;
    else
        *flag2 = 0;

    *segmentnumber = 0;
    for (int i = 3; i>=0; i--)
        *segmentnumber |= ((ptr[0] & (1 << i)) << 2);
    *segmentnumber |= ((ptr[1] & (1 << 7)) >> 6);
    *segmentnumber |= ((ptr[1] & (1 << 6)) >> 6);

    *segmenttotal = (ptr[1] & 0x3f);
}

void ctrl_write(uint8_t encrypted, uint8_t flag1, uint8_t flag2,
        uint8_t segmentnumber, uint8_t segmenttotal, char* ptr) //Reads a segment's control bits
{
    //if (ptr[0] & (0x01 << 7)) //Needs to be one to be a segment but we can ignore
    //Definitely a better way to do the below, but didn't work unless I do it longhand

    ptr[0] = 0;
    ptr[1] = 0;

    ptr[0] |= 1 << 7; //Sets first bit to one to say it's a segment
    ptr[0] |= encrypted << 6;
    ptr[0] |= flag1 << 5;
    ptr[0] |= flag2 << 4;

    ptr[0] |= segmentnumber >> 2;
    ptr[1] |= (segmentnumber & 0x03) << 6;
    ptr[1] |= segmenttotal & 0x3f;
}

void copyin(char* dest, char* source, uint8_t start, uint8_t number, uint16_t srcptr)
{
    for (int i = 0; i < number; i++)
        dest[i+start] = source[i+srcptr];
}

uint8_t waitacknowledge(char dest, char* segment) //returns 1 if needs to go round the loop again
{
    char receivedsegment[MAXMESSAGELENGTH+8] = {0};
    char source;
    uint8_t errorflag = 0; //if any byte is different, errorflag = 1
    uint8_t i;

    unsigned long stime = millis(); //start time
    unsigned long elapsedtime = 0;
    put_string("Time waited (ms):\r\n");
    put_number(elapsedtime);
    put_string("\r");

    while(elapsedtime <= TIMEOUTMILLIS)
    {
        elapsedtime = millis() - stime;
        errorflag = 0;
        for (i = 0; i < MAXMESSAGELENGTH; i++)
            receivedsegment[i] = '\0';

        RecieveSegment(&source, receivedsegment);

        if (strlen(receivedsegment)) //If anything is actually there
        {
            put_string("\r\n\r\nReceiving segment...");
            display_segment(receivedsegment);
            //put_string("\r\n");

            //TODO add function that checks if received source is same as destination

            for (i = 0; i < strlen(segment); i++)
                if (segment[i] != receivedsegment[i])
                    errorflag = 1;

            if (!errorflag)
                return 0;
        }
        else
        {
            put_number(elapsedtime);
            put_string("\r");
        }

    }
    put_string("\r\n");
    return 1;
}