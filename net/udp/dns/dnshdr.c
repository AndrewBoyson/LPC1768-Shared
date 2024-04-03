#include <stdint.h>
#include <stdbool.h>

#include "dns.h"
#include "dnsname.h"
#include "dnslabel.h"
#include "net/net.h"
#include "net/eth/eth.h"
#include "net/ip4/ip4addr.h"
#include "net/ip6/ip6addr.h"
#include "net/udp/udp.h"
#include "log/log.h"

#define HEADER_LENGTH 12

uint16_t DnsHdrId;

bool     DnsHdrIsReply;
bool     DnsHdrIsAuthoritative;
bool     DnsHdrIsRecursiveQuery;

uint16_t DnsHdrQdcount;
uint16_t DnsHdrAncount;
uint16_t DnsHdrNscount;
uint16_t DnsHdrArcount;

char*    DnsHdrPacket;
char*    DnsHdrData;
int      DnsHdrDataLength;

void DnsHdrSetup(void* pPacket, int lenPacket)
{
    DnsHdrPacket = (char*)pPacket;
    DnsHdrData    = DnsHdrPacket + HEADER_LENGTH;
    DnsHdrDataLength = lenPacket - HEADER_LENGTH;
}

void DnsHdrRead()
{
    NetInvert16(&DnsHdrId, DnsHdrPacket + 0);
    
    uint8_t uflags = *(DnsHdrPacket + 2);
    DnsHdrIsReply          = uflags & 0x80;
    DnsHdrIsAuthoritative  = uflags & 0x04;
    DnsHdrIsRecursiveQuery = uflags & 0x01;
    
    NetInvert16(&DnsHdrQdcount, DnsHdrPacket +  4);
    NetInvert16(&DnsHdrAncount, DnsHdrPacket +  6);
    NetInvert16(&DnsHdrNscount, DnsHdrPacket +  8);
    NetInvert16(&DnsHdrArcount, DnsHdrPacket + 10);
}
void DnsHdrWrite()
{
    NetInvert16(DnsHdrPacket + 0, &DnsHdrId);
    
    uint8_t uflags = 0;
    uint8_t lflags = 0;
    if (DnsHdrIsReply)          uflags |= 0x80;
    if (DnsHdrIsAuthoritative)  uflags |= 0x04;
    if (DnsHdrIsRecursiveQuery) uflags |= 0x01;
    *(DnsHdrPacket + 2) = uflags;
    *(DnsHdrPacket + 3) = lflags;
    
    NetInvert16(DnsHdrPacket +  4, &DnsHdrQdcount);
    NetInvert16(DnsHdrPacket +  6, &DnsHdrAncount);
    NetInvert16(DnsHdrPacket +  8, &DnsHdrNscount);
    NetInvert16(DnsHdrPacket + 10, &DnsHdrArcount);

}
static uint16_t decodeUint16(char* p)
{
    uint16_t     value  = *p++;
    value <<= 8; value += *p++;
    return value;
}
static uint32_t decodeUint32(char* p)
{
    uint32_t     value  = *p++;
    value <<= 8; value += *p++;
    value <<= 8; value += *p++;
    value <<= 8; value += *p++;
    return value;
}
static void logRecordA(int len, char* p)
{
    if (len == 4) Ip4AddrLog(*(uint32_t*)p);
    else          LogF("expected 4 bytes but had %d", len);
}
static void logRecordAAAA(int len, char* p)
{
    if (len == 16) Ip6AddrLog(p);
    else           LogF("expected 16 bytes but had %d", len);
}
static void logRecordPTR(int len, char* p)
{
    if (len <= DNS_MAX_LABEL_LENGTH) DnsNameLogPtr(p);
    else LogF("length %d is greater than max DNS label length of %d\r\n", len, DNS_MAX_LABEL_LENGTH);
}
static void logRecordSRV(int len, char* p)
{
    LogF("pri=%d " , decodeUint16(p)); p += 2;
    LogF("wei=%d " , decodeUint16(p)); p += 2;
    LogF("port=%d ", decodeUint16(p)); p += 2;
    DnsNameLogPtr(p);
}
static void logRecordTXT(int len, char* p)
{
    char* pEnd = p + len;
    while (p < pEnd)
    {
        Log("\r\n    ");
        int fieldLen = *p++;
        for (int i = 0; i < fieldLen; i++) LogChar(*p++);
    }
}
static void logContent()
{
    char* p = DnsHdrData;
    
    //Get the questions
    for (int q = 0; q < DnsHdrQdcount; q++)
    {
        if (p >= DnsHdrData + DnsHdrDataLength)
        {
            Log("   Questions have overrun the buffer\r\n");
            return;
        }
        char* pEncodedName = p;
        int nameLength = DnsNameLength(p);
        if (!nameLength) { LogTimeF("   Questions namelength is zero\r\n"); return; }
        p += nameLength;                            //Skip past the name
        p++ ;                                       //skip the first byte of the type
        char recordType = *p++;                     //read the record type
        p += 2;                                     //skip the class

        LogF("   Query ");
        DnsRecordTypeLog(recordType);
        Log(" type record of ");
        DnsNameLogPtr(pEncodedName);
        Log("\r\n");
    }
    
    //Get the answers
    for (int a = 0; a < DnsHdrAncount; a++)
    {
        if (p >= DnsHdrData + DnsHdrDataLength) { Log("  Answers have overrun the buffer\r\n"); return; }
        
        char* pEncodedName = p;
        int nameLength = DnsNameLength(p);
        if (!nameLength) { Log("  Answer name length is zero\r\n"); return; }
        p += nameLength;               //Skip past the name
        
        p++;                           //Skip the high byte of the record type
        int recordType = *p++;         //Record type
        
        p++;                           //Skip the high byte of the class type
        int classType = *p++;          //Class type
        
        int ttl = decodeUint32(p); p += 4;     //32bit TTL
        
        int len = decodeUint16(p); p += 2;     //16bit length
        
        Log("  Answer ");
        DnsRecordTypeLog(recordType);
        Log(" type record of ");
        DnsNameLogPtr(pEncodedName);
        Log(" ==> ");
        
        switch (recordType)           //Log the payload if its type is known
        {
            case DNS_RECORD_A:    logRecordA   (len, p);      break;
            case DNS_RECORD_AAAA: logRecordAAAA(len, p);      break;
            case DNS_RECORD_PTR:  logRecordPTR (len, p);      break;
            case DNS_RECORD_SRV:  logRecordSRV (len, p);      break;
            case DNS_RECORD_TXT:  logRecordTXT (len, p);      break;
            default:              LogF("%d characters", len); break;
        }
        Log("\r\n");
        p += len; //Adjust the pointer to the next character after the payload
    }
}

void DnsHdrLog(int protocol)
{
    if (NetTraceVerbose)
    {
        DnsProtocolLog(protocol);
        Log(" header\r\n");
        LogF("  Ident    %hd\r\n", DnsHdrId);
        if (DnsHdrIsReply)
        {
            if (DnsHdrIsAuthoritative)  LogF("  Authoritative reply\r\n");
            else                        LogF("  Non authoritative reply\r\n");
        }
        else
        {
            if (DnsHdrIsRecursiveQuery) LogF("  Recursive query\r\n");
            else                        LogF("  Non recursive query\r\n");
        }
        LogF("  qd, an, ns, ar  %hu, %hu, %hu, %hu\r\n", DnsHdrQdcount, DnsHdrAncount, DnsHdrNscount, DnsHdrArcount);
    }
    else
    {
        DnsProtocolLog(protocol);
        LogF(" header qd, an, ns, ar  %hu, %hu, %hu, %hu\r\n", DnsHdrQdcount, DnsHdrAncount, DnsHdrNscount, DnsHdrArcount);
    }
    logContent();
}

