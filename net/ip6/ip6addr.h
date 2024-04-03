#include <stdint.h>
#include <stdbool.h>

extern int  Ip6AddrToString(const char* ip, int size, char* text);
extern void Ip6AddrParse   (const char *ip, char *address);
extern int  Ip6AddrLog     (const char* ip);
extern int  Ip6AddrHttp    (const char* ip);

extern bool Ip6AddrIsSame (const char* ipA, const char* ipB);
extern bool Ip6AddrIsEmpty(const char* ip);
extern void Ip6AddrCopy   (char* ipTo, const char* ipFrom);
extern void Ip6AddrClear  (char* ip);

extern bool Ip6AddrIsLinkLocal  (const char* p);
extern bool Ip6AddrIsUniqueLocal(const char* p);
extern bool Ip6AddrIsGlobal     (const char* p);
extern bool Ip6AddrIsExternal   (const char* p);
extern bool Ip6AddrIsSolicited  (const char* p);
extern bool Ip6AddrIsMulticast  (const char *p);
extern bool Ip6AddrIsSameGroup  (const char* pA, const char* pB);

extern const char Ip6AddrAllNodes  [];
extern const char Ip6AddrAllRouters[];
extern const char Ip6AddrMdns      [];
extern const char Ip6AddrLlmnr     [];
extern const char Ip6AddrNtp       [];

extern void Ip6AddrFromDest (int dest , char* pDstIp);

#define SCOPE_NONE         0
#define SCOPE_LINK_LOCAL   1
#define SCOPE_UNIQUE_LOCAL 2
#define SCOPE_GLOBAL       3