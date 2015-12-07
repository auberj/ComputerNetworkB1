#define MAXMESSAGELENGTH 50
#define MAXSEGMENTS 5 //This times the above number shouldn't be less than chars in message
#define TIMEOUTMILLIS 1000 //milliseconds for acknowledgement timeout, then segment is resent

int SendData(char dest, char* sdata, char encryption, char* sessionkey);

int RecieveData(char* source, char* rdata, uint8_t* rmessageflag, char* sessionkey);

void display_segment(char* segment);

void ctrl_read(uint8_t* encryption, uint8_t* flag1, uint8_t* flag2,
        uint8_t* segmentnumber, uint8_t* segmenttotal, char* ptr);

void ctrl_write(uint8_t encryption, uint8_t flag1, uint8_t flag2,
        uint8_t segmentnumber, uint8_t segmenttotal, char* ptr);

void copyin(char* dest, char* source, uint8_t start, uint8_t number, uint16_t srcptr);

uint8_t waitacknowledge(char dest, char* segment);

void swap(char* a, char* b);

void rc4(char *key, char *data);