#include <stdint.h>
#include <stdbool.h>

extern bool DnsSendRequestsViaIp4;

extern void DnsProtocolString  (uint8_t protocol,   int size, char* text);
extern void DnsRecordTypeString(uint8_t recordtype, int size, char* text);
extern void DnsProtocolLog     (uint8_t protocol);
extern void DnsRecordTypeLog   (uint8_t recordtype);

extern void DnsMain(void);
extern int  DnsHandlePacketReceived(void (*traceback)(void), int dnsProtocol, int sizeRx, void* pPacketRx, int* pSizeTx, void* pPacketTx);
extern int  DnsPollForPacketToSend(int ipType, void* pPacket, int* pSize);

#define DNS_UNICAST_SERVER_PORT     53
#define DNS_UNICAST_CLIENT_PORT  53053

#define DNS_MDNS_PORT             5353

#define DNS_LLMNR_SERVER_PORT     5355
#define DNS_LLMNR_CLIENT_PORT    53055

#define DNS_OSDNS_CLIENT_PORT    53057

#define DNS_PROTOCOL_NONE   0
#define DNS_PROTOCOL_UDNS   1
#define DNS_PROTOCOL_MDNS   2
#define DNS_PROTOCOL_OSDNS  3 //Added to allow android to resolve names by one shot mdns see RFC 6762 sections 5.1 and 6.7 - 29/03/2026
#define DNS_PROTOCOL_LLMNR  4

#define DNS_RECORD_NONE  0
#define DNS_RECORD_A     1
#define DNS_RECORD_PTR  12
#define DNS_RECORD_AAAA 28
#define DNS_RECORD_TXT  16
#define DNS_RECORD_SRV  33

