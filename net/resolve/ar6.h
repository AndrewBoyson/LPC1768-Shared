#include <stdbool.h>

extern bool Ar6Trace;

extern void Ar6MakeRequestForMacFromIp(char* ip);

extern int  Ar6AddIpRecord(void (*traceback)(void), char* pMac, char* ip);

extern void Ar6IpToMac(char* ip, char* pMac);
extern void Ar6IndexToIp(int index, char* pIp);
extern bool Ar6HaveMacForIp(char* ip);
extern bool Ar6CheckHaveMacAndFetchIfNot(char* ip);

extern void Ar6SendHttp(void);
extern void Ar6SendAjax(void);

extern void Ar6Main(void);
extern void Ar6Init(void);
