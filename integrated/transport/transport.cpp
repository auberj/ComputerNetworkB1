//Author: dm6g13

#include "transport.h"

int SendData(char dest, char* sdata, char encryption, char* sessionkey)
{
    volatile uint8_t loop = 0;
    uint8_t i;
    uint8_t j;
    uint8_t error;
    uint8_t flag1, flag2;
    uint8_t segmentnumber, segmenttotal;
    uint8_t messagelength;
    uint16_t crcbits;
    uint16_t sdatalength;

	char segment[MAXSEGMENTS][MAXMESSAGELENGTH+8] = {'\0'}; //Initialisation is important

    put_string("\r\n\r\n");
    put_number(strlen(sdata));
    put_string(" character long message from ");
    put_char(callsign);
    put_string(" to ");
    put_char(dest);
    put_string(" reads: \r\n");
    put_string(sdata);

    sdatalength = strlen(sdata);

    uint8_t numberofsegments = (uint8_t)(sdatalength/MAXMESSAGELENGTH);
    if (numberofsegments*MAXMESSAGELENGTH < sdatalength) 
        numberofsegments++;

    if (encryption) //Encrypt data...
        rc4(sessionkey, sdata);

    while (loop < numberofsegments)
    {
        put_string("\r\n\r\nEncoding segment...");
        i = TIMEOUTTRIES;

        for (j = 0; j < MAXMESSAGELENGTH+8; j++)
            segment[loop][j] = 0;

        ctrl_write(encryption, 0, 0, loop+1, numberofsegments, segment[loop]);
        
        segment[loop][2] = 255; //Source port
        segment[loop][3] = 255; //Dest port

        if (loop != numberofsegments - 1)
            messagelength = MAXMESSAGELENGTH;
        else
            messagelength = sdatalength - (loop*MAXMESSAGELENGTH);

        segment[loop][4] = messagelength;
        copyin(segment[loop], sdata, 5, messagelength, (loop*MAXMESSAGELENGTH));
        crcbits = calcrc(segment[loop], messagelength+5);

        segment[loop][messagelength+5] = crcbits >> 8;
        segment[loop][messagelength+6] = crcbits & 0x00FF;

        put_string("segment encoded."); 
    
        display_segment(segment[loop]);
        put_string("\r\n\r\n");

        while(i > 0)
        {
            put_string("\r\n\r\n*******Passing to network layer*******\r\n\r\n");
            SendSegment(dest, segment[loop]);
            put_string("\r\n\r\n*******Returned to transport layer*******\r\n");

            put_string("\r\nSegment sent, waiting on acknowledgment\r\n");

            error = waitacknowledge(dest, segment[loop]); //returns 1 if needs to go round the loop again
            
            if(error)
            {
                i--;
                if (i == 0)
                {
                    //put_string("\r\n\r\nError sending message");
                    return 1; //There was an error sending the segement
                }
                else
                    put_string("\r\nNo acknowledgment, resending segment");
            }
            else
            {
                i = 0;
                put_string("\r\n\r\nAcknowledged.");
                #ifdef transporttest
                globalsegmentnumber = loop + 1;
                #endif
            }
        }

        _delay_ms(1);
        loop = loop + 1;
    }
	return 0;
};

int RecieveData(char* source, char* rdata, uint8_t* rmessageflag, char* sessionkey)
{
    //TODO receive from two different people simulatenously 

    int receiveflag = 0;
    char segment[MAXMESSAGELENGTH+8] = {'\0'};
    uint8_t encryption, flag1, flag2;
    uint8_t segmentnumber, segmenttotal;
    uint16_t crc = 0;

    put_string("\r\n\r\n*******Passing to network layer*******\r\n\r\n");
    receiveflag = RecieveSegment(source, segment);
    put_string("\r\n\r\n*******Returned to transport layer*******\r\n");

    if (receiveflag) //if something has been received
    {
        uint8_t segmentlength = strlen(segment);
        ctrl_read(&encryption, &flag1, &flag2, &segmentnumber, &segmenttotal, segment);

        put_string("\r\n");

        put_number(strlen(segment));
        put_string(" byte long segment from ");
        put_char(*source);
        put_string(" received.");

        display_segment(segment);
        put_string("\r\n");

        uint16_t crc = (segment[strlen(segment) - 2] << 8);
        crc |= (segment[strlen(segment) - 1] & 0x00ff);

        if (crc == calcrc(segment, (strlen(segment)-2))) //if segment is valid
        {
            put_string("\r\nSegent valid, sending acknowledgment\r\n\r\n");
            put_string("*******Passing to network layer*******\r\n\r\n");
            SendSegment(*source, segment); //Acknowledge the segment //TODO acknowledge better
            put_string("\r\n\r\n*******Returned to transport layer*******\r\n");

            #ifdef transporttest
            globalsegmentnumber++;
            #endif

            copyin(rdata, segment, ((segmentnumber - 1)*MAXMESSAGELENGTH), segmentlength-7, 5);
            *rmessageflag = segmenttotal - segmentnumber;

            if (*rmessageflag == 0) //it was the last message so decrypt
            {
                if (encryption) //Decrypt data...
                    rc4(sessionkey, rdata);
            }
        }
    }
    return receiveflag;
}

void display_segment(char* segment)
{
    uint8_t i = 0;
    uint8_t encryption, flag1, flag2;
    uint8_t segmentnumber, segmenttotal;
    uint16_t crc = 0;

    ctrl_read(&encryption, &flag1, &flag2, &segmentnumber, &segmenttotal, segment);

    put_string("\r\n\r\nDecoding segment that is ");
    put_number(strlen(segment));
    put_string(" bytes long.");

    put_string("\r\n\r\nControl Byte 1: ");
    put_binary(segment[0]);
    put_string("\r\nControl Byte 2: ");
    put_binary(segment[1]);

    put_string("\r\nEncrypted: ");
    if (encryption)
        put_string("Yes");
    else
        put_string("No");

    put_string("\r\nFlag1: ");
    put_number(flag1);

    put_string("\r\nFlag2: ");
    put_number(flag2);

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

    if (encryption)
        put_string("\r\nMessage: Encrypted - cannot decode");
    else
    {
        put_string("\r\nMessage: ");
        for (i = 0; i<(strlen(segment)-7); i++)
            put_char(segment[i+5]);
    }

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

void ctrl_read(uint8_t* encryption, uint8_t* flag1, uint8_t* flag2,
        uint8_t* segmentnumber, uint8_t* segmenttotal, char* ptr) //Reads a segment's control bits
{
    if(ptr[0] & (1 << 6))
        *encryption = 1;
    else
        *encryption = 0;

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

void ctrl_write(uint8_t encryption, uint8_t flag1, uint8_t flag2,
        uint8_t segmentnumber, uint8_t segmenttotal, char* ptr) //Reads a segment's control bits
{

    ptr[0] = 0;
    ptr[1] = 0;

    ptr[0] |= 1 << 7; //Sets first bit to one to say it's a segment
    ptr[0] |= encryption << 6;
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

        put_string("\r\n\r\n*******Passing to network layer*******\r\n\r\n");
        RecieveSegment(&source, receivedsegment);
        put_string("\r\n\r\n*******Returned to transport layer*******\r\n");

        if (strlen(receivedsegment)) //If anything is actually there
        {
            put_string("\r\n\r\nReceiving segment...");
            display_segment(receivedsegment);
            //put_string("\r\n");

            if (source != dest)
                errorflag = 1; //Prevents people acknowledging other people's stuff

            for (i = 0; i < strlen(segment); i++)
                if (segment[i] != receivedsegment[i])
                    errorflag = 1;

            if (!errorflag) //If its a valid segment
                return 0;
            else
                put_string("\r\n\r\nInvalid acknowledgment at time (ms): ");
                put_number(elapsedtime);
                put_string("\r\n");
        }
        else
        {
            put_number(elapsedtime);
            put_string("\r\n");
        }

    }
    put_string("\r\n");
    return 1;
}

void swap(uint8_t* a, uint8_t* b)
{
    uint8_t temp;
    temp = *a;
    *a = *b;
    *b = temp;
}

void rc4(char *key, char *data) //Algorithm from https://en.wikipedia.org/wiki/RC4
{
     uint16_t i,j;
     uint8_t S[256];
          
     for (i = 0; i < 256; i++)
        S[i] = i;

     j = 0;

     for (i = 0; i < 256; i++)
     {
        j = (j + S[i] + key[i % strlen(key)]) % 256;    
        swap(&S[i], &S[j]);
     }

     i = 0;
     j = 0;

     for (int k = 0; k < strlen(data); k++)
     {
         i = (i + 1) % 256;
         j = (j + S[i]) % 256;

         swap(&S[i], &S[j]);

         data[k] = data[k] ^ S[(S[i] + S[j]) % 256];
     }
     //data[strlen(data)+1] = '\0';
}