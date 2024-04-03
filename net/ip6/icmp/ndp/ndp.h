#include <stdint.h>
#include <stdbool.h>

extern int      NdpHopLimit;
extern bool     NdpManagedConfiguration;
extern bool     NdpOtherConfiguration;
extern char     NdpRouterMac[6];

extern int      NdpGlobalPrefixLength;
extern bool     NdpGlobalPrefixFlagL;
extern bool     NdpGlobalPrefixFlagA;
extern uint32_t NdpGlobalPrefixValidLifetime;
extern uint32_t NdpGlobalPrefixPreferredLifetime;
extern char     NdpGlobalPrefix[];

extern int      NdpUniqueLocalPrefixLength;
extern bool     NdpUniqueLocalPrefixFlagL;
extern bool     NdpUniqueLocalPrefixFlagA;
extern uint32_t NdpUniqueLocalPrefixValidLifetime;
extern uint32_t NdpUniqueLocalPrefixPreferredLifetime;
extern char     NdpUniqueLocalPrefix[];

extern int      NdpPrefixLength;
extern bool     NdpPrefixFlagL;
extern bool     NdpPrefixFlagA;
extern uint32_t NdpPrefixValidLifetime;
extern uint32_t NdpPrefixPreferredLifetime;
extern char     NdpPrefix[];
extern char     NdpDnsServer[];
extern uint32_t NdpDnsLifetime;
extern int      NdpMtu;

extern int      NdpGetLease(void);
extern void     NdpSetLease(int value);
extern uint32_t NdpGetElapsedLife(void);
extern bool     NdpIsFresh(void);

extern int  NdpAddOptionSourceMac(char* p, char* pMac);
extern int  NdpAddOptionTargetMac(char* p, char* pMac);
extern void NdpDecodeOptions    (char* pData, int dataLength, char* srcMac, char* dstMac);
extern void NdpLogOptionsVerbose(char* pData, int dataLength);
extern void NdpLogOptionsQuiet  (char* pData, int dataLength);
