#include <stdbool.h>
#include <stdint.h>

extern bool UdpTrace;

extern int  UdpHandleReceivedPacket(void (*traceback)(void), int sizeRx, char* pPacketRx, int* pSizeTx, char* pPacketTx);
extern int  UdpPollForPacketToSend(int type, int* pSize, char* pPacket);

extern void UdpLogHeader(uint16_t calculatedChecksum);

extern void UdpMakeHeader(int size, char* pPacket);
extern void UdpHdrSetChecksum(void*pPacket, uint16_t checksum);
