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


#include "DL_makeframe.cpp"







int bytestuff(char *str, int len) {
    int i;
    char temp[HEADERLEN + CONTROLLEN + ADDRESSLEN + LENGTHLEN + DATALEN + CHECKSUMLEN + FOOTERLEN + 10] = "";
    for(i = 0; i<len; i++) {
        if(str[i] == HEADER || str[i] == ESCAPE) {
            // put_number(int(str[i]));
            // put_char('\n');
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

int test() {
    return 0;
}

int SendPacket(char dest, char* Spacket) {
    /*
    Packet length = 122  => max 6 frames
    split packet into frames, send frame, await acknowledgement  
    */
    struct frame data[FRAMECOUNT];
    int no_frames;
    put_number(millis());
    put_char('\n');
    no_frames = makeframe(&data, dest, Spacket, 0);
    put_number(millis());
    put_char('\n');    

    uint8_t *bufptr;
    char temp[50];

    int i,k, send_complete;
    struct frame ack;
    unsigned long time;

    for(i = 0; i < no_frames; i++) {
        send_complete = 0;
        char test[FRAMELEN] = "0123456789012345678901234";
        while(!send_complete) {
            ///////////////////send//////////////////////
            rfm12_tx(strlen(data[i].frame), 0, (uint8_t*)data[i].frame);
            for (uint8_t j = 0; j < 100; j++)   
            {   
                //put_string(". ");
                rfm12_tick();   
                _delay_us(500); 
            }

            time = millis() + 500;

            while((millis() != time)) {
                ///////////check for acknowledgemt/////////////////
                if(rfm12_rx_status() == STATUS_COMPLETE) {
                    //send_complete = 1;
                    bufptr = rfm12_rx_buffer();
                    for(k = 0; k < (rfm12_rx_len()); k++) {
                        temp[k] = bufptr[k];
                    }
                    temp[rfm12_rx_len()] = '\0';
                    rfm12_rx_clear();
                    // put_string("\nRECEIVED: ");
                    // put_string(temp);
                    // put_string("\n\n");
                    ////////////////check if acknowledgemnt valid////////////////
                    int check_ack = decode_frame(&ack, temp);
                    if((check_ack & (1<<1)) && !(check_ack & 1<<2)) {
                        //if(strcmp(ack.data, data[i].data)) {
                        put_string("\nSend Complete!\n");
                        send_complete = 1;
                        break;  
                        //}
                    }
                 }
            }
            if(!send_complete) {
                put_string("\nTIMEOUT\n");
            }

        }
    }

    /*
    for frame in frames:
        send_complete = 0;
        while not send complete:
            send frame
            start timer
            while timer:
                if acknowledgement:
                    send_complete = 1;

    */





    return 0;
}

int RecievePacket(char* Rpacket) {
    /*
    see http://www.hansinator.de/rfm12lib/ for rfm12b libray details
    receive frame, send acknowledgement

    if received a frame:
        de-bytestuff
        check crc
        if recipient:
            acknowledge
            pass to network
    */
    int i = 0;
    if (rfm12_rx_status() == STATUS_COMPLETE) {
        uint8_t* bufptr;
        char Rframe[50], ackstr[50];
        struct frame ack, decode;
        struct frame Nrframe[FRAMECOUNT];
        struct frame ackarr[FRAMECOUNT];
        int Received_Final_frame = 0;
        int timeout = millis() + 1000;
        while(!Received_Final_frame && (millis() < timeout)){
            //int Rframe_len;
            if (rfm12_rx_status() == STATUS_COMPLETE) {
                bufptr = rfm12_rx_buffer();
                for(uint8_t k = 0; k < (rfm12_rx_len()); k++) {
                    Rframe[k] = bufptr[k];
                }
                Rframe[rfm12_rx_len()] = '\0';
                rfm12_rx_clear();
                // put_string("\r\nRframe: ");
                // put_string(Rframe);
                //strcpy(ackstr, Rframe);
                decode = Nrframe[i];
                int Rframe_check = decode_frame(&decode, Rframe);
                Nrframe[i] = decode;
                // put_string("\r\nRframe.address: ");
                // put_string(Nrframe[i].address);
                // put_string("\r\nRframe.length");
                // put_number(Nrframe[i].length[0]);
                // put_string("\r\nNrframe[i]: ");
                // put_number(i);
                // put_string("\r\nRframe.data: ");
                // put_string(Nrframe[i].data);
                // put_string("\r\nRframe_check: ");
                // put_number(Rframe_check);
                if(Rframe_check & (1<<1)) {
                    if(Rframe_check & 1<<4) {
                        Received_Final_frame = 1;
                    }
                    put_string("\r\nFrame Number; ");
                    put_number(i);
                    /*
                    frame received, frame for me
                    acknowledge
                    */
                    ackarr[0] = Nrframe[i];
                    makeframe(&ackarr, Nrframe[i].address[0], Nrframe[i].data, 1, 1);
                    put_string("\r\nacknowledgement: ");
                    put_string(ackarr[0].frame);
                    rfm12_tx(strlen(ackarr[0].frame), 0, (uint8_t*)ackarr[0].frame);
                    for (uint8_t j = 0; j < 100; j++)   
                    {   
                        //put_string(". ");
                        rfm12_tick();   
                        _delay_us(500); 
                    }
                    i++;
                    timeout = millis() + 1000;
                    
                }
                else if(!Rframe_check) {
                    ;
                }
                else {
                    break;
                }

            }
        }
        if(i && i < FRAMECOUNT) {
            put_string("\r\nPacketComplete\n\r");
            strcpy(Rpacket, Nrframe[0].data);
            for(int l = 1; l<i; l++) {
                strcat(Rpacket, Nrframe[l].data);
            }
            strcat(Rpacket, "\0");
        }
    }
    return i;

}

void unbytestuff(char *Rframe) {
    uint16_t i;
    int cnt = 0;
    // put_number(strlen(Rframe));
    for(i = 0; i < strlen(Rframe); i++) {
        if(Rframe[i] == ESCAPE) {
            i++;
        }
        Rframe[cnt++] = Rframe[i];
    }
    Rframe[cnt] = 0;
}

int decode_frame(struct frame *framedata, char * Rframe) {
    /*
    return 0 if error
    
    bits in return set to specify different things

    bit 0: No errors
    bit 1: for me
    bit 2: set = INFOFRAME, not set = SUPEFRAME 
    bit 3: first frame
    bit 4: last frame

    */
    int retval = 0;
    if(Rframe[0] == HEADER && Rframe[strlen(Rframe)-1] == FOOTER ) {
        unbytestuff(Rframe);
        strncpy(Rframe, Rframe, strlen(Rframe)-1);
        Rframe[strlen(Rframe)-1] = '\0';
        if(!((long unsigned int)calccrc(Rframe, strlen(Rframe)))) {
            //put_string("\nNo Errors!\n");
            retval |= 1;
            int i;
            
            for(i = 0; i < 10; i++) {
                if(i < CONTROLLEN) {
                    framedata->control[i] = Rframe[HEADERLEN + i]; 
                }
                if(i < ADDRESSLEN) {
                    framedata->address[i] = Rframe[HEADERLEN + CONTROLLEN + i];
                }
                if(i < LENGTHLEN) {
                    framedata->length[i] = Rframe[HEADERLEN + CONTROLLEN + ADDRESSLEN + i];
                }
            }
            framedata->control[CONTROLLEN] = 0;
            framedata->address[ADDRESSLEN] = 0;
            framedata->length[LENGTHLEN] = 0;
            for(i = 0; i < framedata->length[0]; i++) {


                framedata->data[i] = Rframe[HEADERLEN + CONTROLLEN + ADDRESSLEN + LENGTHLEN + i];
            }

            //get rid of start and end bits//////
            // if(Rframe[HEADERLEN + CONTROLLEN + ADDRESSLEN + LENGTHLEN] == START) {
            //     strcpy(framedata->data, framedata->data);
            // }
            // if(framedata->data[(int)framedata->length[0] - 1] == END) {
            //     framedata->data[(int)framedata->length[0] - 1] = 0;
            // }
            /////////////////////////////////////

            framedata->data[(int)framedata->length[0]] = 0;
            put_string("\r\nRframe.data: ");
            put_string(framedata->data);
            if(framedata->address[1] == BROADCAST || framedata->address[1] == THISDEVICE) {
                put_string("\r\nPacket for me!");
                retval |= 1 << 1;
                if(framedata->data[0] == START) {
                    retval |= 1<< 3;
                    strcpy(framedata->data, framedata->data);

                }
                if(framedata->data[strlen(framedata->data)-1] == END) {
                    retval |= 1<<4;
                    framedata->data[(int)framedata->length[0] - 1] = 0;
                }
                if(!strcmp(framedata->control, INFOFRAME)) {
                    retval |= 1 << 2;
                }
            }
            else {
                // not intended recepient
                put_string("\r\nNot for me!\n");
                //retval = 1;
            }


        }
        else {
            // error in transmission
            //retval = 1;
            ;
        }

    }



    
    return retval;

}
