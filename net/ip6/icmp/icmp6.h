extern int Icmp6HandleReceivedPacket(void (*traceback)(void), int scope, char* pPacketRx, int sizeRx, char* pPacketTx, int* pSizeTx, char* pSrcIp, char* pDstIp);
extern int Icmp6PollForPacketToSend(                                                                  char* pPacket,   int* pSize,   char* pSrcIp, char* pDstIp);
