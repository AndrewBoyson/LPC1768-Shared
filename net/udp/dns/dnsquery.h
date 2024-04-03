#include <stdint.h>
#include <stdbool.h>

extern bool DnsQueryTrace;

extern uint32_t DnsQueryIp4;
extern char     DnsQueryIp6[];
extern char     DnsQueryName[];

extern void     DnsQueryMain(void);

extern bool     DnsQueryIsBusy;

extern void     DnsQueryIp4FromName(char * name, int dnsProtocol, int ipProtocol);
extern void     DnsQueryNameFromIp4(uint32_t ip, int dnsProtocol, int ipProtocol);

extern void     DnsQueryIp6FromName(char * name, int dnsProtocol, int ipProtocol);
extern void     DnsQueryNameFromIp6(char* ip,    int dnsProtocol, int ipProtocol);

extern int      DnsQueryPoll(int ipType, void* pPacket, int* pSize);
