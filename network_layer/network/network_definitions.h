//control bit definitions
#define Control1Hello 'H'
#define Control2Hello 'O'

#define Control1Neighbour 'N'

#define Control1Message 'M'

//source address (my address)
#define SCRADD 'N'

#define MaxSegmentLength 0x79

//checksum definitions
#define HelloChecksum 0x00

#define NumNeighbours 10 //can have up to ten neighbours

#define HelloPacketLength 15
#define MaxPacketLength 128

#define NumOldPackets 20 //the number of old packets to check for retransmission multiplied by 2