extern bool     ArpTrace;

extern uint32_t ArpAddressToResolve;
extern bool     ArpResolveRequestFlag;

extern int ArpHandleReceivedPacket(void (*traceback)(void), void* pPacketRx, int sizeRx, void* pPacketTx, int* pSizeTx);
extern int ArpPollForPacketToSend(                                                       void* pPacketTx, int* pSizeTx);
