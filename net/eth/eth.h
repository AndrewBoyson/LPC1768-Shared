#include <stdint.h>

extern uint16_t EthProtocol;
extern char*    EthMacRemote;

extern void EthProtocolLog(uint16_t prototype);
extern int  EthHandlePacket       (char* pPacketRx, int sizeRx, char* pPacketTx, int* pSizeTx);
extern int  EthPollForPacketToSend(char* pPacket, int* pSize);

#define ETHERNET  1

#define ETH_NONE 0
#define ETH_IPV4 0x0800
#define ETH_ARP  0x0806
#define ETH_IPV6 0x86DD

