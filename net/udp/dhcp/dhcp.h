#include <stdint.h>
#include <stdbool.h>

extern bool DhcpTrace;

extern uint32_t DhcpGetElapsedLife(void);

extern uint32_t DhcpLeaseTime;
extern uint32_t DhcpServerIp;
extern uint32_t DhcpRouterIp;
extern uint32_t DhcpSubnetMask;
extern uint32_t DhcpNtpIp;
extern uint32_t DhcpRenewalT1;
extern uint32_t DhcpRenewalT2;
extern uint32_t DhcpBroadcastIp;
extern uint32_t DhcpLocalIp;
extern uint32_t DhcpDnsServerIp;
extern char     DhcpDomainName[];
extern char     DhcpHostName[];

extern bool     DhcpIpNeedsToBeRouted(uint32_t ip);

extern int      DhcpHandleResponse(void (*traceback)(void), int sizeRx, void* pPacketRx, int* pSizeTx, void* pPacketTx);
extern int      DhcpPollForRequestToSend(void* pPacket, int* pSize);

#define DHCP_SERVER_PORT 67
#define DHCP_CLIENT_PORT 68