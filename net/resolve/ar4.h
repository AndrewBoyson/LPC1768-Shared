#include <stdint.h>
#include <stdbool.h>

extern bool Ar4Trace;

extern void Ar4MakeRequestForMacFromIp(uint32_t ip);

extern int  Ar4AddIpRecord(void (*traceback)(void), char* pMac, uint32_t ip);

extern void     Ar4IpToMac(uint32_t ip, char* pMac);
extern uint32_t Ar4GetIpFromMac(char* pMac);
extern uint32_t Ar4IndexToIp(int index);
extern bool     Ar4HaveMacForIp(uint32_t ip);
extern bool     Ar4CheckHaveMacAndFetchIfNot(uint32_t ip);

extern void     Ar4SendHttp(void);
extern void     Ar4SendAjax(void);

extern void Ar4Main(void);
extern void Ar4Init(void);
