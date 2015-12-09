#include "DataLink.h"
// #include <stdio.h>
// #include <avr/io.h>
// #include <string.h>


void setchecksum(struct frame (*vals)[FRAMECOUNT]) {
    int i;
    for(i = 0; i<FRAMECOUNT; i++) {
        if((*vals)[i].length[0]) {
            char checksumcalc[50] = "";
            strcpy(checksumcalc,(*vals)[i].header);
            strcat(checksumcalc,(*vals)[i].control);
            strcat(checksumcalc,(*vals)[i].address);
            strcat(checksumcalc,(char*)(*vals)[i].length);
            //strcat(checksumcalc,(char*)(*vals)[i].header);
            strcat(checksumcalc,(*vals)[i].data);
            
            uint16_t crc = calcrc(checksumcalc, strlen(checksumcalc));

            (*vals)[i].checksum[0] = (uint8_t)(crc>>8);
            (*vals)[i].checksum[1] = (uint8_t)(crc & 0xff);
            (*vals)[i].checksum[2] = '\0';
            // char temp[30];
            // sprintf(temp, "crc = %x, %x %x\n",crc, (*vals)[i].checksum[0],(*vals)[i].checksum[1]);
            // put_string(temp);
        }
    }
}

void setdata(struct frame (*vals)[FRAMECOUNT], char* Spacket, int ack) {
    /*
        vals is an array of frame structs size FRAMECOUNT, and fills the frame.data
    */
    //put_number(strlen(Spacket));
    put_string("\r\n>>>start setdata:\r\n");
    int cnt = 0;
    int i, j, end;
    end = 0;
    //char temp[DATALEN];
    for(i = 0; i < FRAMECOUNT-1; i++) {
        // int loop = DATALEN;
        //
        for(j = 0; j < DATALEN; j++) {
            // if(i ==0 && j == 0 && !ack) {
            //     (*vals)[i].data[j++] = START;
            //     //loop--;
            // }
            if(Spacket[cnt] == '\0' ) {
                // if(!ack) {
                //     (*vals)[i].data[j] = END;
                //     j++;                    
                // }
                (*vals)[i].lastframe = 1;
                //put_char((*vals)[i].data[j]);
                end = 1;
                break;
            }
            (*vals)[i].data[j] = Spacket[cnt++];
            put_char((*vals)[i].data[j]);
        }
        // put_number(j);
        (*vals)[i].length[0] = j;
        (*vals)[i].data[j] = '\0';
        if(end) {
            break;
        }
        //put_string((*vals)[i].data);
        //put_char('\n');
    }
    put_string("\r\n>>>End setdata\r\n");

}

void setheader(struct frame (*vals)[FRAMECOUNT]) {
    int i;
    for(i = 0; i < FRAMECOUNT; i++) {
        (*vals)[i].header[0] = HEADER;
        (*vals)[i].header[1] = '\0';
        //put_char((*vals)[i].header[0]);
    }
}

void setfooter(struct frame (*vals)[FRAMECOUNT]) {
    int i;
    for(i = 0; i < FRAMECOUNT; i++) {
        (*vals)[i].footer[0] = FOOTER;
        (*vals)[i].footer[1] = '\0';        
        //put_char((*vals)[i].footer[0]);
    }
}

void dataInit(struct frame (*vals)[FRAMECOUNT]) {
    int i;
    for(i = 0; i < FRAMECOUNT; i++) {
        (*vals)[i].length[0] = 0;
        (*vals)[i].length[1] = '\0';
        (*vals)[i].header[1] = '\0';
        (*vals)[i].footer[1] = '\0';
        (*vals)[i].lastframe = 0;
    }    
}

void setaddress(struct frame (*vals)[FRAMECOUNT], char address) {
    int i;
    int j;
    for(i = 0; i < FRAMECOUNT; i++) {
        if((*vals)[i].length[0]) {
            (*vals)[i].address[0] = callsign;
            (*vals)[i].address[1] = address;
            (*vals)[i].address[ADDRESSLEN] = '\0';
        }
    }    
}

void setcontrol(struct frame (*vals)[FRAMECOUNT], int ack) {
    put_string("\r\n>>>Start setcontrol\r\n");
    int i;
    int j;
    for(i = 0; i < FRAMECOUNT; i++) {
        if((*vals)[i].length[0]) {
            
            if(!ack) {
                (*vals)[i].control[0] = 'I';
                if(i == 0) {
                    (*vals)[i].control[1] = '1';
                }
                else if((*vals)[i].lastframe) {
                    (*vals)[i].control[1] = '0';
                }
                else {
                    (*vals)[i].control[1] = i;
                }
            }
            else {
                (*vals)[i].control[j] = SUPEFRAME[j];
            }

                //put_char((*vals)[i].control[j]);
            
            (*vals)[i].control[CONTROLLEN] = '\0';
            put_string("\r\nControl: ");
            put_string((*vals)[i].control);
        }
    } 
    put_string("\r\n>>>End setcontrol\r\n");   
}

int makeframe(struct frame (*data)[FRAMECOUNT], char dest, char*Spacket, int ack, int frames = FRAMECOUNT) {
    dataInit(data);    

    setdata(data, Spacket, ack);

    setheader(data);

    setcontrol(data, ack);

    setaddress(data, dest);    

    setchecksum(data);

    setfooter(data);
    int i;
    int retval = 0;
    for(i = 0; i < frames; i++) {
        if((*data)[i].length[0]) {
            // put_string("\nMAke the string: \n");
            char temp[HEADERLEN + CONTROLLEN + ADDRESSLEN + LENGTHLEN + DATALEN + CHECKSUMLEN + FOOTERLEN + 10] = "";
            sprintf(temp,"%s%s%s%s%s",(*data)[i].control,(*data)[i].address,(*data)[i].length,(*data)[i].data,(*data)[i].checksum);

            bytestuff(temp, strlen(temp));

            strcpy((*data)[i].frame, (*data)[i].header);
            strcat((*data)[i].frame, temp);
            strcat((*data)[i].frame, (*data)[i].footer);

            // put_string((*data)[i].frame);
            // put_char('\n');
            // put_number(strlen((*data)[i].frame));
            // put_char('\n');
            retval = i;

        }
    }
    return retval+1;  
}