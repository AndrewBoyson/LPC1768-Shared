#include <stdbool.h>

extern bool NsTraceRecvSol;
extern bool NsTraceRecvAdv;
extern bool NsTraceSendSol;

extern char NsAddressToResolve[];
extern bool NsResolveRequestFlag;

extern int  NsHandleReceivedAdvertisement(void (*traceback)(void), void* pPacket, int* pSize);
extern int  NsHandleReceivedSolicitation (void (*traceback)(void), uint8_t* pType, uint8_t* pCode, void* pPacketRx, int sizeRx, void* pPacketTx, int* pSizeTx);
extern int  NsGetWaitingSolicitation     (void* pPacket, int* pSize, uint8_t* pType, uint8_t* pCode);