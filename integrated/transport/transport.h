#define MAXMESSAGELENGTH 20
#define MAXSEGMENTS 5 //This times the above number shouldn't be less than chars in message
#define TIMEOUTMILLIS 500 //milliseconds for acknowledgement timeout, then segment is resent
#define TIMEOUTTRIES 2 //Number of timeouts before it says there's an error

int SendData(char dest, char* sdata, char encryption, char* sessionkey);

void RecieveData(char* source, char* rdata, char* sessionkey);

void display_segment(char* segment);

void ctrl_read(uint8_t* encryption, uint8_t* flag1, uint8_t* flag2,
        uint8_t* segmentnumber, uint8_t* segmenttotal, char* ptr);

void ctrl_write(uint8_t encryption, uint8_t flag1, uint8_t flag2,
        uint8_t segmentnumber, uint8_t segmenttotal, char* ptr);

void copyin(char* dest, char* source, uint8_t start, uint8_t number, uint16_t srcptr);

uint8_t waitacknowledge(char dest, char* segment);

void swap(char* a, char* b);

void rc4(char *key, char *data);