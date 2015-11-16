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
#include "lcd/avrlcd.h"
#include "lcd/font.c"
#include "lcd/ili934x.c"
#include "lcd/lcd.c"
#include "DataLink.h"
//using namespace std;

int main() {
    init_lcd();
    set_orientation(East);
    display_string((char*)"Initialising...\n");
    
    char random[] = "This is a short string that should be 122 characters long, and it continues, it will continue, abcdefghijklmnopqrstuvxyz2";
    SendPacket('a', random);

    return 0;

}

void setdata(struct frame (*vals)[FRAMECOUNT], char* Spacket) {
    /*
        vals is an array of frame structs size FRAMECOUNT, and fills the frame.data
    */

    int i, j;
    //char temp[DATALEN];
    for(i = 0; i < FRAMECOUNT-1; i++) {
        for(j = 0; j < DATALEN; j++) {
            (*vals)[i].data[j] = Spacket[i*DATALEN + j];
            display_char(Spacket[i*DATALEN + j]);
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

void setcontrol(struct frame (*vals)[FRAMECOUNT], char* address) {
    int i;
    int j;
    for(i = 0; i < FRAMECOUNT; i++) {
        for(j = 0; j < ADDRESSLEN; j++) {
            (*vals)[i].address[j] = address[j];
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
    

    //correctly set data
    display_string((char*)"setdata\n");
    setdata(&data, Spacket);
    display_string((char*)"setheader \n");
    setheader(&data);
    

    return 0;
}
int RecievePacket(char* Rpacket) {
    /*
    receive frame, send acknowledgement
    */
    return 0;

}

