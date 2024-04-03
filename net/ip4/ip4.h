
extern bool     Ip4Trace;
extern uint32_t Ip4Remote;
extern int      Ip4HandleReceivedPacket(void (*traceback)(void), void* pPacketRx, int sizeRx, void* pPacketTx, int* pSizeTx, char* pRemoteMac);
extern int      Ip4PollForPacketToSend (                                                      void* pPacketTx, int* pSizeTx, char* pRemoteMac);
