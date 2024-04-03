extern int  Udp6HandleReceivedPacket(void (*traceback)(void), int scope, void* pPacketRx, int sizeRx, void* pPacketTx, int* pSizeTx, char* pSrcIp, char* pDstIp);
extern int  Tcp6HandleReceivedPacket(void (*traceback)(void), int scope, void* pPacketRx, int sizeRx, void* pPacketTx, int* pSizeTx, char* pSrcIp, char* pDstIp, int remArIndex);
extern int  Udp6PollForPacketToSend (                                                                 void* pPacket,   int* pSize,   char* pSrcIp, char* pDstIp);
extern int  Tcp6PollForPacketToSend (                                                                 void* pPacket,   int* pSize,   char* pSrcIp, char* pDstIp);
