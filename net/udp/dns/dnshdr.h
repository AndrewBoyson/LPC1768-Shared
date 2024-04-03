#include <stdint.h>
#include <stdbool.h>

extern uint16_t DnsHdrId;

extern bool     DnsHdrIsReply;
extern bool     DnsHdrIsAuthoritative;
extern bool     DnsHdrIsRecursiveQuery;

extern uint16_t DnsHdrQdcount;
extern uint16_t DnsHdrAncount;
extern uint16_t DnsHdrNscount;
extern uint16_t DnsHdrArcount;

extern char*    DnsHdrPacket;
extern char*    DnsHdrData;
extern int      DnsHdrDataLength;

extern void     DnsHdrSetup(void* pPacket, int lenPacket);
extern void     DnsHdrRead (void);
extern void     DnsHdrWrite(void);
extern void     DnsHdrLog  (int ipProtocol);
