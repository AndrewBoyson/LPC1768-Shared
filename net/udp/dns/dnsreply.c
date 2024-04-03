#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "log/log.h"
#include "net/net.h"
#include "net/action.h"
#include "net/ip4/ip4.h"
#include "net/ip6/ip6.h"
#include "net/resolve/nr.h"
#include "dns.h"
#include "dnshdr.h"
#include "dnsname.h"
#include "dnslabel.h"
#include "net/udp/dhcp/dhcp.h"

bool DnsReplyTrace = false;

char     DnsReplyRecordName[256];
uint32_t DnsReplyRecordNameAsIp4 = 0;
char     DnsReplyRecordNameAsIp6[16];
char     DnsReplyName[64];
uint32_t DnsReplyIp4 = 0;
char     DnsReplyIp6[16];

static char* p;
static char* pRecordName;
static int    recordType;
static int    recordDataLength;
static char* pRecordData;

static int scanQuery()
{
    int recordNameLength = DnsNameLength(p);
    if (!recordNameLength) return -1; //failure
    
    pRecordName = p;
    p += recordNameLength;
    
    p++ ; //skip the first byte of the type
    recordType = *p++;
    
    p += 2; //skip the class
    
    return 0; //success
}

static int scanAnswer()
{
    int recordNameLength = DnsNameLength(p);
    if (!recordNameLength) return -1; //failure
    
    pRecordName = p;
    p += recordNameLength;
    
    p++ ; //skip the first byte of the type
    recordType = *p++;
    
    p += 6; //skip the class, TTL
    recordDataLength = 0;
    recordDataLength |= *p++ <<  8;
    recordDataLength |= *p++;
    
    pRecordData = p; //record the start of the data
    
    p += recordDataLength; //Move to the start of the next record
    
    return 0; //success
}
static void readAnswer()
{
    DnsReplyRecordName[0]      = 0;
    DnsReplyRecordNameAsIp4    = 0;
    DnsReplyRecordNameAsIp6[0] = 0;
    DnsReplyName[0]            = 0;
    DnsReplyIp4                = 0;
    DnsReplyIp6[0]             = 0;
    
    switch (recordType)
    {
        case DNS_RECORD_A:
        case DNS_RECORD_AAAA:
        case DNS_RECORD_PTR:
        case DNS_RECORD_SRV:
        case DNS_RECORD_TXT:
            break;
        default:
            return;
    }

    DnsNameDecodePtr(pRecordName,  DnsReplyRecordName, sizeof(DnsReplyRecordName));
    DnsNameDecodeIp4(pRecordName, &DnsReplyRecordNameAsIp4);
    DnsNameDecodeIp6(pRecordName,  DnsReplyRecordNameAsIp6);

    switch (recordType)
    {
        case DNS_RECORD_A:
            if (recordDataLength != 4) return;
            memcpy(&DnsReplyIp4, pRecordData, 4);
            break;
        case DNS_RECORD_AAAA:
            if (recordDataLength != 16) return;
            memcpy(DnsReplyIp6, pRecordData, 16);
            break;
        case DNS_RECORD_PTR:
            if (recordDataLength > DNS_MAX_LABEL_LENGTH) return;
            DnsNameDecodePtr(pRecordData, DnsReplyName, sizeof(DnsReplyName));
            break;
    }
}

static void sendToDnsCache(int dnsProtocol)
{
    char strippedName[100];
    if (DnsReplyRecordName[0]) DnsLabelStripNameFromFullName(dnsProtocol, DnsReplyRecordName, sizeof(strippedName), strippedName);
    if (DnsReplyName[0]      ) DnsLabelStripNameFromFullName(dnsProtocol, DnsReplyName      , sizeof(strippedName), strippedName);
    
    if (DnsReplyIp4                && DnsReplyRecordName[0]) NrAddAddress4(DnsReplyIp4,             strippedName, dnsProtocol);
    if (DnsReplyIp6[0]             && DnsReplyRecordName[0]) NrAddAddress6(DnsReplyIp6,             strippedName, dnsProtocol);
    if (DnsReplyRecordNameAsIp4    && DnsReplyName[0]      ) NrAddAddress4(DnsReplyRecordNameAsIp4, strippedName, dnsProtocol);
    if (DnsReplyRecordNameAsIp6[0] && DnsReplyName[0]      ) NrAddAddress6(DnsReplyRecordNameAsIp6, strippedName, dnsProtocol);
}
int DnsReplyHandle(void (*traceback)(void), int dnsProtocol)
{
    bool ok = true;
    if (!DnsHdrAncount) ok = false;

    p = DnsHdrData;
    
    if (DnsReplyTrace || NetTraceHostGetMatched())
    {
        if (NetTraceNewLine) Log("\r\n");
        LogTimeF("DnsReply received\r\n");
        if (NetTraceStack) traceback();
        DnsHdrLog(dnsProtocol);
    }

    if (ok)
    {
        for (int q = 0; q < DnsHdrQdcount; q++)
        {
            if (scanQuery())
            {
                ok = false;
                break;
            }
        }
    }
    if (ok)
    {
        for (int a = 0; a < DnsHdrAncount; a++)
        {
            if (scanAnswer())
            {
                ok = false;
                break;
            }
            readAnswer();
            sendToDnsCache(dnsProtocol);
        }
    }
    
    NetTraceHostCheckIp6(DnsReplyIp6);
    NetTraceHostCheckIp6(DnsReplyRecordNameAsIp6);
    
    return DO_NOTHING;
}
