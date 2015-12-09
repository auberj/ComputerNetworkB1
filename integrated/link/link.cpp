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

void put_frame(struct frame pframe) {
    put_string("\n\r>>>Start print frame");        
    put_string("\r\nsend control: ");
    put_string(pframe.control);
    put_string("\r\naddress: ");
    put_string(pframe.address);
    put_string("\r\ndata: ");
    put_string(pframe.data);
    put_string("\r\nLength: ");
    put_number(pframe.length[0]);
    put_string("\r\nChecksum:");
    put_number(pframe.checksum[0]);
    put_char(' ');
    put_number(pframe.checksum[1]);
    put_string("\r\nSend Frame: ");
    put_string(pframe.frame);
    put_string("\n\r>>>End Print Frame\r\n");
}

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

int test() {
    return 0;
}

int SendPacket(char dest, char* Spacket) {
    /*
    Packet length = 122  => max 6 frames
    split packet into frames, send frame, await acknowledgement  
    */
    put_string("\r\nstring passed to DLL: ");
    put_string(Spacket);
    struct frame data[FRAMECOUNT];
    int no_frames;
    no_frames = makeframe(&data, dest, Spacket, 0);
    
    put_string("\r\n\nnumber of frames generated: ");
    put_number(no_frames);

    uint8_t *bufptr;
    char temp[50];

    int i,k, send_complete;
    struct frame ack;
    unsigned long time;

    for(i = 0; i < no_frames; i++) {
        send_complete = 0;
        if((uint8_t)dest != (uint8_t)BROADCAST) {
            while(!send_complete) {
                ///////////////////send//////////////////////
                put_frame(data[i]);

                SendFrame(data[i].frame);


                time = millis() + 1000;

                while((millis() != time)) {
                    ///////////check for acknowledgemt/////////////////
                    //put_char(':');
                    if(ReceiveFrame()) {
                        //send_complete = 1;
                        ReceiveFrame(temp);
                        // bufptr = rfm12_rx_buffer();
                        // for(k = 0; k < (rfm12_rx_len()); k++) {
                        //     temp[k] = bufptr[k];
                        // }
                        // temp[rfm12_rx_len()] = '\0';
                        // rfm12_rx_clear();
                        // put_string("\r\nRECEIVED: ");
                        // put_string(temp);
                        // put_string("\r\n\r\n");
                        ////////////////check if acknowledgemnt valid////////////////
                        int check_ack = decode_frame(&ack, temp);
                        if((check_ack & (1<<1)) && !(check_ack & 1<<2)) {
                            //if(strcmp(ack.data, data[i].data)) {
                            put_string("\r\nSend Complete!\r\n");
                            send_complete = 1;
                            break;  
                            //}
                        }
                     }
                }
                if(!send_complete) {
                    put_string("\r\ntimeout\r\n");
                }

            }
        }
        else {
            put_frame(data[i]);

            SendFrame(data[i].frame);

            _delay_ms(400);
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
    if (ReceiveFrame()) { //Status complete 1 - passes this section
        put_char('.');
        uint8_t* bufptr;
        char Rframe[50], ackstr[50];
        struct frame ack, decode;
        struct frame Nrframe[FRAMECOUNT];
        struct frame ackarr[FRAMECOUNT];
        int Received_Final_frame = 0;
        unsigned long timeout = millis() + 20000;
        while(!Received_Final_frame && (millis() < timeout)){ //never passes this while statement
                                                                //Also maybe add RFM12B tick??
            //int Rframe_len;
            put_string("Trying to receive data");
            if (ReceiveFrame()) { //Status complete 2 - why?

                ReceiveFrame(Rframe);
                // bufptr = rfm12_rx_buffer();
                // put_string("Raw data received: ");
                // put_string((char*)bufptr);
                // for(uint8_t k = 0; k < (rfm12_rx_len()); k++) {
                //     Rframe[k] = bufptr[k];
                // }
                // Rframe[rfm12_rx_len()] = '\0';
                // rfm12_rx_clear();
                put_string("\r\nRframe: ");
                put_string(Rframe);
                //strcpy(ackstr, Rframe);
                decode = Nrframe[i];
                int Rframe_check = decode_frame(&decode, Rframe);
                Nrframe[i] = decode;
                put_string("\r\nReceived Data: ");
                put_frame(Nrframe[i]);

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
                    if(!(Rframe_check & 1<<5)) {
                        put_string("\n\r>>>>START MAKE AND SEND ACKNOWLEDGEMENT\r\n");
                        ackarr[0] = Nrframe[i];

                        int framecount = makeframe(&ackarr, Nrframe[i].address[0], Nrframe[i].data, 1, 1);
                        put_string("\r\nNumber of acknowledgement frames generated: ");
                        put_number(framecount);
                        put_string("\r\nacknowledgement: ");
                        put_frame(ackarr[0]);
                        //_delay_ms(100);
                        SendFrame(ackarr[0].frame);
                        // rfm12_tx(strlen(ackarr[0].frame), 0, (uint8_t*)ackarr[0].frame);
                        // for (uint8_t j = 0; j < 100; j++)   
                        // {   
                        //     //put_string(". ");
                        //     rfm12_tick();   
                        //     _delay_us(500); 
                        // }
                    }
                    i++;
                    timeout = millis() + 20000;
                    
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
            put_string("\r\nReceived packet: ");
            put_string(Rpacket);
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
    bit 5: Broadcast 
    */
    put_string("\r\n>>>Start decode_frame\r\n");
    int retval = 0;
    if(Rframe[0] == HEADER && Rframe[strlen(Rframe)-1] == FOOTER ) {
        // put_string("\r\nBefore unbytestuff: ");
        // put_string(Rframe);
        unbytestuff(Rframe);
        // put_string("\r\nafter unbytestuff: ");
        // put_string(Rframe);
        strncpy(Rframe, Rframe, strlen(Rframe)-1);
        // put_string("\r\nRemoved footer: ");
        // put_string(Rframe);
        // put_string("\r\nstrlen: ");
        // put_number(strlen(Rframe));
        Rframe[strlen(Rframe)-1] = '\0';
        // put_string("\r\nadd zero: ");
        // put_string(Rframe);
        // put_string("\r\nstrlen: ");
        // put_number(strlen(Rframe));
        uint16_t crc = (Rframe[strlen(Rframe) - 2] << 8);
        crc |= (Rframe[strlen(Rframe) - 1] & 0x00ff);
        //put_hex(crc, 2);

        //put_string("\r\nSegment Valid: ");
        //put_hex(calcrc(segment, (strlen(segment)-2)), 2);
        if(crc == calcrc(Rframe, (strlen(Rframe)-2))) {
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
            if((uint8_t)framedata->address[1] == (uint8_t)BROADCAST || (uint8_t)framedata->address[1] == (uint8_t)callsign) {
                put_string("\r\nFrame for me!");
                retval |= 1 << 1;
                if(framedata->control[1] == START) {
                    put_string("\r\nFirst Frame\r\n");
                    retval |= 1<< 3;
                    // strcpy(framedata->data, framedata->data+1);

                }
                if(framedata->control[1] == END) {
                    put_string("\r\nLast Frame\r\n");
                    retval |= 1<<4;
                    // framedata->data[(int)framedata->length[0] - 1] = 0;
                }
                if(framedata->control[0] == INFOFRAME[0]) {
                    put_string("\r\nInfoFrame\r\n");
                    retval |= 1 << 2;
                }
                if((uint8_t)framedata->address[1] == (uint8_t)BROADCAST) {
                    put_string("\r\nBroadcast\r\n");
                    retval |= 1<<5;
                }
            }
            else {
                // not intended recepient
                put_string("\r\nDestination address: ");
                put_number(framedata->address[1]);
                put_string("\r\nBroadcast address: ");
                put_number(BROADCAST);
                put_string("\r\nNot for me!\n");
                //retval = 1;
            }


        }
        else {


            // error in transmission
            //retval = 1;
            put_string("\r\nCRC failed");
        }

    }


    put_string("\r\n>>>End decode_frame\r\n");

    
    return retval;

}