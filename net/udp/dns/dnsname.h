extern bool DnsNameTrace;

extern bool DnsNameComparePtr(char* pStart, char* pName);
extern bool DnsNameCompareIp4(char* pStart, uint32_t ip);
extern bool DnsNameCompareIp6(char* pStart, char*   pIp);

extern int  DnsNameLength    (char* pStart);

extern void DnsNameDecodePtr (char* pStart, char* pName, int lenName);
extern void DnsNameDecodeIp4 (char* pStart, uint32_t* pIp);
extern void DnsNameDecodeIp6 (char* pStart,     char* pIp);

extern void DnsNameLogPtr    (char* pStart);

extern void DnsNameEncodeIp4(uint32_t ip, char** pp);
extern void DnsNameEncodeIp6(char* ip,    char** pp);
extern void DnsNameEncodePtr(char* pName, char** pp);
