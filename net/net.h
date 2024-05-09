#include <stdint.h>
#include <stdbool.h>

extern char* NetName;

extern bool NetTraceStack;
extern bool NetTraceNewLine;
extern bool NetTraceVerbose;
extern char NetTraceHost[];
extern bool NetTraceHostGetMatched(void);
extern void NetTraceHostResetMatched(void);
extern void NetTraceHostCheckIp6(char* ip);
extern void NetTraceHostCheckMac(char* mac);

extern int16_t NetToHost16(int16_t n);
extern int32_t NetToHost32(int32_t n);
extern int64_t NetToHost64(int64_t n);
extern void    NetInvert16(void* h, void* n);
extern void    NetInvert32(void* h, void* n);
extern void    NetInvert64(void* h, void* n);
extern void    NetDirect16(void* h, void* n);
extern void    NetDirect32(void* h, void* n);
extern void    NetDirect64(void* h, void* n);

extern void NetInit(char* name, char* linkLedPin, char* speedLedPin);
extern void NetMain(void);
