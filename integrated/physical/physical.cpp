#include "rfm12_config.h"
#include "./rfm12lib/rfm12.h"
#include "./rfm12lib/rfm12.cpp"
 
void SendFrame(char *SFrame) {
    rfm12_tx(strlen(SFrame), 0, (uint8_t*)SFrame);
    for (uint8_t j = 0; j < 100; j++)   
    {   
        put_string(". ");
        rfm12_tick();   
        _delay_us(500); 
    }
}

int ReceiveFrame(char *Rframe = 0) {
	int retval;
	uint8_t *bufptr;
	if(rfm12_rx_status() == STATUS_COMPLETE){
		if(Rframe != 0) {
			bufptr = rfm12_rx_buffer();
            for(int k = 0; k < (rfm12_rx_len()); k++) {
                Rframe[k] = bufptr[k];
            }
            Rframe[rfm12_rx_len()] = '\0';
            rfm12_rx_clear();
		}
		retval = 1;
	}
	else {
		retval = 0;
	}
	return retval;
}