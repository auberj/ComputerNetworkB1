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
#include "DataLink.h"
#include "DL_makeframe.cpp"



int main() {
    init_lcd();
    set_orientation(East);
    display_string((char*)"Initialising...\n");
    
    char random[120] = "hello aaron rowland, this is a string which should be split among many frames, aaron is a s";

    SendPacket('\xff', random);
    return 0;

}

int bytestuff(char *str, int len) {
    int i;
    char temp[HEADERLEN + CONTROLLEN + ADDRESSLEN + LENGTHLEN + DATALEN + CHECKSUMLEN + FOOTERLEN + 10] = "";
    for(i = 0; i<len; i++) {
        if(str[i] == HEADER || str[i] == ESCAPE) {
            // display_number(int(str[i]));
            // display_char('\n');
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



int SendPacket(char dest, char* Spacket) {
    /*
    Packet length = 122  => max 6 frames
    split packet into frames, send frame, await acknowledgement  
    */
    struct frame data[FRAMECOUNT];
    makeframe(&data, dest, Spacket);





    return 0;
}
int RecievePacket(char* Rpacket) {
    /*
    receive frame, send acknowledgement
    */
    return 0;

}

