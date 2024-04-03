#include <stdint.h>

uint16_t UserUdpPort1 = 0;
uint16_t UserUdpPort2 = 0;
int (*UserHandleReceivedUdpPacket)(uint16_t port, void (*traceback)(void), int dataLengthRx, char* pDataRx, int* pPataLengthTx, char* pDataTx) = 0;


uint16_t UserUdpDstPort = 0;
uint16_t UserUdpSrcPort = 0;
uint32_t UserIp4;
char     UserMac[6];

int (*UserPollForUdpPacketToSend)(int type, int* pDataLength, char* pData);