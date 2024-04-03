extern int      Ip4AddrToString  (const uint32_t ip, int size, char* text);
extern int      Ip4AddrLog       (const uint32_t ip);
extern int      Ip4AddrHttp      (const uint32_t ip);
extern uint32_t Ip4AddrParse     (const char* text);

extern void     Ip4AddrFromDest  (const int dest, uint32_t* pDstIp);

extern bool     Ip4AddrIsExternal(uint32_t ip);

#define IP4_BROADCAST_ADDRESS       0xFFFFFFFF
#define IP4_MULTICAST_ALL_HOSTS     0x010000E0
#define IP4_MULTICAST_ALL_ROUTERS   0x020000E0
#define IP4_MULTICAST_DNS_ADDRESS   0xFB0000E0
#define IP4_MULTICAST_LLMNR_ADDRESS 0xFC0000E0
#define IP4_MULTICAST_NTP_ADDRESS   0x010100E0

