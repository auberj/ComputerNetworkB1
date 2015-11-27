
#include "DataLink.h"
#include <stdio.h>
#include <avr/io.h>
#include <string.h>


void setchecksum(struct frame (*vals)[FRAMECOUNT]) {
    int i;
    for(i = 0; i<FRAMECOUNT; i++) {
        if((*vals)[i].length[0]) {
            char checksumcalc[50] = "";
            strcpy(checksumcalc,(*vals)[i].header);
            strcat(checksumcalc,(*vals)[i].control);
            strcat(checksumcalc,(*vals)[i].address);
            strcat(checksumcalc,(char*)(*vals)[i].length);
            strcat(checksumcalc,(char*)(*vals)[i].header);
            strcat(checksumcalc,(*vals)[i].data);
            
            uint16_t crc = calccrc(checksumcalc, strlen(checksumcalc));

            (*vals)[i].checksum[0] = (uint8_t)(crc>>8);
            (*vals)[i].checksum[1] = (uint8_t)(crc & 0xff);
            (*vals)[i].checksum[2] = '\0';
            // char temp[30];
            // sprintf(temp, "crc = %x, %x %x\n",crc, (*vals)[i].checksum[0],(*vals)[i].checksum[1]);
            // display_string(temp);
        }
    }
}

void setdata(struct frame (*vals)[FRAMECOUNT], char* Spacket) {
    /*
        vals is an array of frame structs size FRAMECOUNT, and fills the frame.data
    */
    //display_number(strlen(Spacket));
    int cnt = 0;
    int i, j, end;
    end = 0;
    //char temp[DATALEN];
    for(i = 0; i < FRAMECOUNT-1; i++) {
        // int loop = DATALEN;
        //
        for(j = 0; j < DATALEN; j++) {
            if(i ==0 && j == 0) {
                (*vals)[i].data[j++] = START;
                //loop--;
            }
            if(Spacket[cnt] == '\0') {
                (*vals)[i].data[j] = END;
                display_char((*vals)[i].data[j]);
                end = 1;
                j++;
                break;
            }
            (*vals)[i].data[j] = Spacket[cnt++];
            display_char((*vals)[i].data[j]);
        }
        // display_number(j);
        (*vals)[i].length[0] = j;
        (*vals)[i].data[j] = '\0';
        if(end) {
            break;
        }
        //display_string((*vals)[i].data);
        display_char('\n');
    }
}

void setheader(struct frame (*vals)[FRAMECOUNT]) {
    int i;
    for(i = 0; i < FRAMECOUNT; i++) {
        (*vals)[i].header[0] = HEADER;
        (*vals)[i].header[1] = '\0';
        display_char((*vals)[i].header[0]);
    }
}

void setfooter(struct frame (*vals)[FRAMECOUNT]) {
    int i;
    for(i = 0; i < FRAMECOUNT; i++) {
        (*vals)[i].footer[0] = FOOTER;
        (*vals)[i].footer[1] = '\0';        
        display_char((*vals)[i].footer[0]);
    }
}

void dataInit(struct frame (*vals)[FRAMECOUNT]) {
    int i;
    for(i = 0; i < FRAMECOUNT; i++) {
        (*vals)[i].length[0] = 0;
        (*vals)[i].length[1] = '\0';
        (*vals)[i].header[1] = '\0';
        (*vals)[i].footer[1] = '\0';
    }    
}

void setaddress(struct frame (*vals)[FRAMECOUNT], char* address) {
    int i;
    int j;
    for(i = 0; i < FRAMECOUNT; i++) {
        if((*vals)[i].length[0]) {
            for(j = 0; j < ADDRESSLEN; j++) {
                (*vals)[i].address[j] = address[j];
                display_char((*vals)[i].address[j]);
            }
            (*vals)[i].address[ADDRESSLEN] = '\0';
        }
    }    
}

void setcontrol(struct frame (*vals)[FRAMECOUNT]) {
    int i;
    int j;
    for(i = 0; i < FRAMECOUNT; i++) {
        if((*vals)[i].length[0]) {
            for(j = 0; j < CONTROLLEN; j++) {
                (*vals)[i].control[j] = INFOFRAME[j];
                display_char((*vals)[i].control[j]);
            }
            (*vals)[i].control[CONTROLLEN] = '\0';
        }
    }    
}

void makeframe(struct frame (*data)[FRAMECOUNT], char dest, char*Spacket) {
    dataInit(data);    

    setdata(data, Spacket);

    setheader(data);

    setcontrol(data);

    setaddress(data, (char*)"FF");    

    setchecksum(data);

    setfooter(data);
    int i;
    for(i = 0; i < FRAMECOUNT; i++) {
        if((*data)[i].length[0]) {
            display_string("\nMAke the string: \n");
            char temp[HEADERLEN + CONTROLLEN + ADDRESSLEN + LENGTHLEN + DATALEN + CHECKSUMLEN + FOOTERLEN + 10] = "";
            sprintf(temp,"%s%s%s%s%s",(*data)[i].control,(*data)[i].address,(*data)[i].length,(*data)[i].data,(*data)[i].checksum);

            bytestuff(temp, strlen(temp));

            strcpy((*data)[i].frame, (*data)[i].header);
            strcat((*data)[i].frame, temp);
            strcat((*data)[i].frame, (*data)[i].footer);

            display_string((*data)[i].frame);
            display_char('\n');
            display_number(strlen((*data)[i].frame));
            display_char('\n');

        }
    }  
}
