
extern bool Ip6Trace;

extern int  Ip6HandleReceivedPacket(void (*traceback)(void), void* pPacketRx, int sizeRx, void* pPacketTx, int* pSizeTx, char* pRemMac);
extern int  Ip6PollForPacketToSend (                                                      void* pPacketTx, int *pSizeTx, char* pRemMac);
