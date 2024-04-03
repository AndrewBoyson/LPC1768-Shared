#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "dns.h"
#include "dnshdr.h"
#include "dnsquery.h"
#include "dnsreply.h"
#include "dnsserver.h"
#include "log/log.h"
#include "net/udp/dhcp/dhcp.h"


bool DnsSendRequestsViaIp4 = false;

void DnsProtocolString(uint8_t protocol, int size, char* text)
{
    switch (protocol)
    {
        case DNS_PROTOCOL_UDNS:  strncpy (text, "DNS",   size);        break;
        case DNS_PROTOCOL_MDNS:  strncpy (text, "MDNS",  size);        break;
        case DNS_PROTOCOL_LLMNR: strncpy (text, "LLMNR", size);        break;
        default:                 snprintf(text, size, "%d", protocol); break;
    }
}

void DnsRecordTypeString(uint8_t recordtype, int size, char* text)
{
    switch (recordtype)
    {
        case DNS_RECORD_A:    strncpy (text, "A",    size);      break;
        case DNS_RECORD_AAAA: strncpy (text, "AAAA", size);      break;
        case DNS_RECORD_PTR:  strncpy (text, "PTR",  size);      break;
        case DNS_RECORD_TXT:  strncpy (text, "TXT",  size);      break;
        case DNS_RECORD_SRV:  strncpy (text, "SRV",  size);      break;
        default:              snprintf(text, size, "%d", recordtype); break;
    }
}
void DnsProtocolLog(uint8_t protocol)
{
    switch (protocol)
    {
        case DNS_PROTOCOL_UDNS:  Log ("DNS  ");        break;
        case DNS_PROTOCOL_MDNS:  Log ("MDNS ");        break;
        case DNS_PROTOCOL_LLMNR: Log ("LLMNR");        break;
        default:                 LogF("%d", protocol); break;
    }
}

void DnsRecordTypeLog(uint8_t recordtype)
{
    switch (recordtype)
    {
        case DNS_RECORD_A:    Log ("A"   );           break;
        case DNS_RECORD_AAAA: Log ("AAAA");           break;
        case DNS_RECORD_PTR:  Log ("PTR" );           break;
        case DNS_RECORD_TXT:  Log ("TXT" );           break;
        case DNS_RECORD_SRV:  Log ("SRV" );           break;
        default:              LogF("%d", recordtype); break;
    }
}

void DnsMain()
{
    DnsQueryMain();
}

int DnsHandlePacketReceived(void (*traceback)(void), int dnsProtocol, int sizeRx, void* pPacketRx, int* pSizeTx, void* pPacketTx)
{
    DnsHdrSetup(pPacketRx, sizeRx);
    DnsHdrRead();
    
    int action;
    if (DnsHdrIsReply)
    {
        action = DnsReplyHandle(traceback, dnsProtocol);
    }
    else
    {
        action = DnsServerHandleQuery(traceback, dnsProtocol, pPacketTx, pSizeTx);
    }
    return action;
}

int DnsPollForPacketToSend(int ipType, void* pPacketTx, int* pSize)
{
    return DnsQueryPoll(ipType, pPacketTx, pSize);
}