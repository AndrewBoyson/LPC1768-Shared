#include <stdint.h>

extern uint16_t UserUdpPort1;
extern uint16_t UserUdpPort2;
extern int (*UserHandleReceivedUdpPacket)(uint16_t port, void (*traceback)(void), int dataLengthRx, char* pDataRx, int* pPataLengthTx, char* pDataTx);


extern uint16_t UserUdpDstPort;
extern uint16_t UserUdpSrcPort;
extern uint32_t UserIp4;
extern char     UserMac[6];

extern int (*UserPollForUdpPacketToSend)(int type, int* pDataLength, char* pData);