extern int Udp4HandleReceivedPacket(void (*traceback)(void), void* pPacketRx, int sizeRx, void* pPacketTx, int* pSizeTx, uint32_t* pSrcIp, uint32_t* pDstIp);
extern int Tcp4HandleReceivedPacket(void (*traceback)(void), void* pPacketRx, int sizeRx, void* pPacketTx, int* pSizeTx, uint32_t* pSrcIp, uint32_t* pDstIp, int remArIndex);
extern int Udp4PollForPacketToSend (                                                      void* pPacketTx, int* pSizeTx, uint32_t* pSrcIp, uint32_t* pDstIp);
extern int Tcp4PollForPacketToSend (                                                      void* pPacketTx, int* pSizeTx, uint32_t* pSrcIp, uint32_t* pDstIp);
