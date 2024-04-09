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

extern void NetInit(char* name, void (* linkLed)(char on), void (*speedLed)(char on), char ntpEnable);
extern void NetMain(void);

#define FAULT_POINT_NetMain                   1
#define FAULT_POINT_LinkMain                  2
#define FAULT_POINT_EthHandlePacket           3
#define FAULT_POINT_Ip4HandleReceivedPacket   4
#define FAULT_POINT_Tcp4HandleReceivedPacket  5
#define FAULT_POINT_TcpHandleReceivedPacket   6
#define FAULT_POINT_TcpHdrReadFromPacket      7
#define FAULT_POINT_HttpHandleRequest         8
#define FAULT_POINT_HttpReadRequest           9
#define FAULT_POINT_HttpRequestFunction      10
#define FAULT_POINT_HttpPostFunction         11
#define FAULT_POINT_HttpGetFunction          12
#define FAULT_POINT_Udp4HandleReceivedPacket 13
#define FAULT_POINT_UdpHandleReceivedPacket  14
#define FAULT_POINT_NtpHandlePacketReceived  15