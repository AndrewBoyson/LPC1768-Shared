extern bool DnsServerTrace;

extern int DnsServerHandleQuery(void (*traceback)(void), int dnsProtocol, void* pPacketTx, int *pSize);
