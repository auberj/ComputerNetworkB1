/*
    The data link layer

    receives a packet from the network layer and provides a reliable link to the next transceiver, ensuring the
    data arrives in the order it was sent. Acts like a wire.

    Frame Structure:

    | Header[1] | control[2] | Addressing[2] | length[1] | Data[1-23] | Checksum[2] | Foter[1] |

    Features:
        Acknowledged connectionless link.
            - probable look into connection oriented at a later date

        Framing Method - bit stuffing (have to check what is currently being implemented in the rfm library)

        Flow control - probably not needed

        error control - maybe

*/

#include <stdio.h>
#include <avr/io.h>
#include <string.h>
#include "lcd/avrlcd.h"
#include "lcd/font.c"
#include "lcd/ili934x.c"
#include "lcd/lcd.c"
#include "DataLink.h"



int main() {
    init_lcd();
    set_orientation(East);
    display_string((char*)"Initialising...\n");
    
    char random[40] = {'1','2','3','4','5','6','7','8','9','a','b','c','d','e','f','g','h','i','j','k'};

    SendPacket('\xff', random);
    return 0;

}

int bytestuff(char *str, int len) {
    int i;
    char temp[100] = "";
    for(i = 0; i<len; i++) {
        if(str[i] == HEADER || str[i] == ESCAPE) {
            display_number(int(str[i]));
            display_char('\n');
            sprintf(temp, "%s%c", temp, ESCAPE);
        }
        sprintf(temp,"%s%c", temp, str[i]);
    }
    strcpy(str,temp);
    return strlen(str);
}

uint16_t calccrc(char *str, int len) {
    int i, r;
    r = 0;
    for(i = 0; i < len; i++) {
        r = r ^ (str[i] << 8);
        int j;
        for(j = 0; j < 8; j++) {
            r = (r&0x8000)? ((r<<1)^GENERATOR):(r<<1);
            r = r & 0xffff;
        } 
    }
    return r;
}

void setchecksum(struct frame (*vals)[CHECKSUMLEN]) {
    int i;
    for(i = 0; i<FRAMECOUNT; i++) {
        char checksumcalc[CONTROLLEN + ADDRESSLEN + LENGTHLEN + DATALEN];
        sprintf(checksumcalc, "%c%s%s%c%s", (*vals[i]).header,(*vals[i]).control, (*vals[i]).address, (*vals[i]).length, (*vals[i]).data);
        
    }
}

void setdata(struct frame (*vals)[FRAMECOUNT], char* Spacket) {
    /*
        vals is an array of frame structs size FRAMECOUNT, and fills the frame.data
    */

    int i, j;
    //char temp[DATALEN];
    for(i = 0; i < FRAMECOUNT-1; i++) {
        for(j = 0; j < DATALEN; j++) {
            if(Spacket[i*DATALEN + j] == 0) {
                break;
            }
            (*vals)[i].data[j] = Spacket[i*DATALEN + j];
            display_char(Spacket[i*DATALEN + j]);
        }
        if(Spacket[i*DATALEN + j] == 0) {
            break;
        }
        //display_string((*vals)[i].data);
        display_char('\n');
    }
}

void setheader(struct frame (*vals)[FRAMECOUNT]) {
    int i;
    for(i = 0; i < FRAMECOUNT; i++) {
        (*vals)[i].header = HEADER;
        display_char((*vals)[i].header);
    }
}

void setfooter(struct frame (*vals)[FRAMECOUNT]) {
    int i;
    for(i = 0; i < FRAMECOUNT; i++) {
        (*vals)[i].footer = FOOTER;
        display_char((*vals)[i].footer);
    }
}

void setlength(struct frame (*vals)[FRAMECOUNT]) {
    int i;
    for(i = 0; i < FRAMECOUNT; i++) {
        (*vals)[i].length = FRAMECOUNT;
        display_char((*vals)[i].length);
    }    
}

void setaddress(struct frame (*vals)[FRAMECOUNT], char* address) {
    int i;
    int j;
    for(i = 0; i < FRAMECOUNT; i++) {
        for(j = 0; j < ADDRESSLEN; j++) {
            (*vals)[i].address[j] = address[j];
            display_char((*vals)[i].address[j]);
        }
    }    
}

void setcontrol(struct frame (*vals)[FRAMECOUNT]) {
    int i;
    int j;
    for(i = 0; i < FRAMECOUNT; i++) {
        for(j = 0; j < ADDRESSLEN; j++) {
            (*vals)[i].control[j] = INFOFRAME[j];
            display_char((*vals)[i].address[j]);
        }
    }    
}


int SendPacket(char dest, char* Spacket) {
    /*
    Packet length = 122  => max 6 frames
    split packet into frames, send frame, await acknowledgement  
    */
    struct frame data[FRAMECOUNT];
    
    display_string((char*)"\nsetheader \n");
    setheader(&data);

    display_string((char*)"\nsetcontrol\n");
    setcontrol(&data);

    display_string((char*)"\nsetaddress \n");
    setaddress(&data, (char*)"FF");

    //correctly set data
    display_string((char*)"\nsetdata\n");
    setdata(&data, Spacket);
    
    display_string((char*)"\nsetlength \n");
    setlength(&data);

    display_string((char*)"\nsetfooter \n");
    setfooter(&data);




    return 0;
}
int RecievePacket(char* Rpacket) {
    /*
    receive frame, send acknowledgement
    */
    return 0;

}

