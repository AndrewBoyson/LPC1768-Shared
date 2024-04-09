#include <stdbool.h>
#include <string.h>

#include "dns.h"
#include "dnshdr.h"
#include "dnsname.h"
#include "dnslabel.h"
#include "net/net.h"
#include "net/action.h"
#include "log/log.h"
#include "net/udp/dhcp/dhcp.h"
#include "net/ip6/slaac.h"
#include "net/ip4/ip4.h"
#include "net/ip6/ip6.h"

bool DnsServerTrace = false;

#define RECORD_NONE               0
#define RECORD_PTR4               1
#define RECORD_PTR6_LINK_LOCAL    2
#define RECORD_PTR6_UNIQUE_LOCAL  3
#define RECORD_PTR6_GLOBAL        4
#define RECORD_A                  5
#define RECORD_AAAA_LINK_LOCAL    6
#define RECORD_AAAA_UNIQUE_LOCAL  7
#define RECORD_AAAA_GLOBAL        8

#define MAX_ANSWERS 4

//Set by 'initialise'
static char* p;               //Position relative to DnsHdrData and is updated while both reading questions and writing answers
static char  myFullName[100]; //The name, adjusted to include the domain if needed by the protocol, used when reading and when writing
static int   myFullNameLength;

//Set by readQuestions and used by answerQuestions
static int   answers[MAX_ANSWERS];
static int   answerCount = 0;
static bool  mdnsUnicastReply;

static int readQuestions()
{
    p = DnsHdrData;
    mdnsUnicastReply = false;
    
    //Get the questions
    answerCount = 0;
    for (int i = 0; i < DnsHdrQdcount; i++)
    {
        //Bomb out if there are too many answers
        if (answerCount >= MAX_ANSWERS)
        {
            if (DnsServerTrace) LogTimeF("DnsServer-readQuestions - exceeded %d answers\r\n", MAX_ANSWERS);
            break;
        }
        
        //Bomb out if we are tending to overrun the buffer
        if (p - DnsHdrData > DnsHdrDataLength)
        {
            if (DnsServerTrace) LogTimeF("DnsServer-readQuestions - overrunning the buffer of %d bytes\r\n", DnsHdrDataLength);
            return -1;
        }
        
        //Node name
        int nameLength = DnsNameLength(p);
        if (!nameLength)
        {
            if (DnsServerTrace) LogTimeF("DnsServer-readQuestions namelength is zero\r\n");
            return -1;
        }
        bool nodeIsName  = DnsNameComparePtr(p, myFullName);
        bool nodeIsAddr4 = DnsNameCompareIp4(p, DhcpLocalIp);
        bool nodeIsLocl6 = DnsNameCompareIp6(p, SlaacLinkLocalIp);
        bool nodeIsUniq6 = DnsNameCompareIp6(p, SlaacUniqueLocalIp);
        bool nodeIsGlob6 = DnsNameCompareIp6(p, SlaacGlobalIp);
        p += nameLength;                          //Skip past the name
                
        //Type
        p++ ;                                     //skip the first byte of the type
        char recordType = *p++;                   //read the record type
        
        //Class
        if (*p++ & 0x80) mdnsUnicastReply = true; //check the class 15th bit (UNICAST-RESPONSE)
        p++;                                      //skip the class
        
        //Handle the questions
        if (nodeIsName && recordType == DNS_RECORD_A   )   answers[answerCount++] = RECORD_A;
        if (nodeIsName && recordType == DNS_RECORD_AAAA)
        {
                                                           answers[answerCount++] = RECORD_AAAA_LINK_LOCAL;
                                if (SlaacUniqueLocalIp[0]) answers[answerCount++] = RECORD_AAAA_UNIQUE_LOCAL;
                                if (SlaacGlobalIp     [0]) answers[answerCount++] = RECORD_AAAA_GLOBAL;
        }
        if (nodeIsAddr4 && recordType == DNS_RECORD_PTR )  answers[answerCount++] = RECORD_PTR4;
        if (nodeIsLocl6 && recordType == DNS_RECORD_PTR )  answers[answerCount++] = RECORD_PTR6_LINK_LOCAL;
        if (nodeIsUniq6 && recordType == DNS_RECORD_PTR )  answers[answerCount++] = RECORD_PTR6_UNIQUE_LOCAL;
        if (nodeIsGlob6 && recordType == DNS_RECORD_PTR )  answers[answerCount++] = RECORD_PTR6_GLOBAL;
    }
    return 0;
}
static int addAnswers(int dnsProtocol)
{    
    //Go through each answer
    DnsHdrAncount = 0;
    for (int i = 0; i < answerCount; i++)
    {                
        //Bomb out if we are tending to overrun the buffer
        if (p - DnsHdrData > DnsHdrDataLength)
        {
            if (DnsServerTrace) LogTimeF("DnsServer-addAnswers - reply is getting too big\r\n");
            return -1;
        }
                
        //Encode the node name
        switch (answers[i])
        {
            case RECORD_A:                 DnsNameEncodePtr(myFullName,         &p); break;
            case RECORD_AAAA_LINK_LOCAL:   DnsNameEncodePtr(myFullName,         &p); break;
            case RECORD_AAAA_UNIQUE_LOCAL: DnsNameEncodePtr(myFullName,         &p); break;
            case RECORD_AAAA_GLOBAL:       DnsNameEncodePtr(myFullName,         &p); break;
            case RECORD_PTR4:              DnsNameEncodeIp4(DhcpLocalIp,        &p); break;
            case RECORD_PTR6_LINK_LOCAL:   DnsNameEncodeIp6(SlaacLinkLocalIp,   &p); break;
            case RECORD_PTR6_UNIQUE_LOCAL: DnsNameEncodeIp6(SlaacUniqueLocalIp, &p); break;
            case RECORD_PTR6_GLOBAL:       DnsNameEncodeIp6(SlaacGlobalIp,      &p); break;
        }
        
        //Add the 16 bit type
        *p++ = 0;
        switch (answers[i])
        {
            case RECORD_A:                 *p++ = DNS_RECORD_A;    break;
            case RECORD_AAAA_LINK_LOCAL:   *p++ = DNS_RECORD_AAAA; break;
            case RECORD_AAAA_UNIQUE_LOCAL: *p++ = DNS_RECORD_AAAA; break;
            case RECORD_AAAA_GLOBAL:       *p++ = DNS_RECORD_AAAA; break;
            case RECORD_PTR4:              *p++ = DNS_RECORD_PTR;  break;
            case RECORD_PTR6_LINK_LOCAL:   *p++ = DNS_RECORD_PTR;  break;
            case RECORD_PTR6_UNIQUE_LOCAL: *p++ = DNS_RECORD_PTR;  break;
            case RECORD_PTR6_GLOBAL:       *p++ = DNS_RECORD_PTR;  break;
        }
        
        //Add the class
        char mdns = dnsProtocol == DNS_PROTOCOL_MDNS ? 0x80 : 0; //Set the 15th bit (CACHE_FLUSH) of the class to 1 if MDNS
        *p++ = mdns; *p++ = 1;                                   //16 bit Class LSB QCLASS_IN = 1 - internet
        
        //Add the TTL
        *p++ =    0; *p++ = 0; *p++ = 4; *p++ = 0;               //32 bit TTL seconds - 1024
        
        
        //Add the 16 bit payload length
        *p++ = 0;
        switch (answers[i])
        {
            case RECORD_A:                 *p++ =  4;                   break;
            case RECORD_AAAA_LINK_LOCAL:   *p++ = 16;                   break;
            case RECORD_AAAA_UNIQUE_LOCAL: *p++ = 16;                   break;
            case RECORD_AAAA_GLOBAL:       *p++ = 16;                   break;
            case RECORD_PTR4:              *p++ = myFullNameLength + 2; break; //add a byte for the initial length and another for the terminating zero length
            case RECORD_PTR6_LINK_LOCAL:   *p++ = myFullNameLength + 2; break;
            case RECORD_PTR6_UNIQUE_LOCAL: *p++ = myFullNameLength + 2; break;
            case RECORD_PTR6_GLOBAL:       *p++ = myFullNameLength + 2; break;
        }
        
        //Add the payload
        switch (answers[i])
        {
            case RECORD_A:                 memcpy(p, &DhcpLocalIp,        4); p +=  4; break;
            case RECORD_AAAA_LINK_LOCAL:   memcpy(p, SlaacLinkLocalIp,   16); p += 16; break;
            case RECORD_AAAA_UNIQUE_LOCAL: memcpy(p, SlaacUniqueLocalIp, 16); p += 16; break;
            case RECORD_AAAA_GLOBAL:       memcpy(p, SlaacGlobalIp,      16); p += 16; break;
            case RECORD_PTR4:              DnsNameEncodePtr(myFullName, &p);           break;
            case RECORD_PTR6_LINK_LOCAL:   DnsNameEncodePtr(myFullName, &p);           break;
            case RECORD_PTR6_UNIQUE_LOCAL: DnsNameEncodePtr(myFullName, &p);           break;
            case RECORD_PTR6_GLOBAL:       DnsNameEncodePtr(myFullName, &p);           break;
        }
        //Increment the number of good answers to send
        DnsHdrAncount++;
    }
    return 0;
}

int DnsServerHandleQuery(void (*traceback)(void), int dnsProtocol, void* pPacketTx, int *pSizeTx) //Received an mdns or llmnr query on port 5353 or 5355
{            
    myFullNameLength = DnsLabelMakeFullNameFromName(dnsProtocol, NetName, sizeof(myFullName), myFullName);
    
    if (readQuestions()) return DO_NOTHING;
    if (!answerCount) return DO_NOTHING;
    
    if (DnsServerTrace || NetTraceHostGetMatched())
    {
        if (NetTraceNewLine) Log("\r\n");
        LogTimeF("DnsServer received query\r\n");
        if (NetTraceStack) traceback();
        DnsHdrLog(dnsProtocol);
    }
    
    char* pRx = DnsHdrData;
    char* pEndRx = p;
    int qdcount = DnsHdrQdcount;
    int nscount = DnsHdrNscount;
    int arcount = DnsHdrArcount;
    
    DnsHdrSetup(pPacketTx, *pSizeTx);
    p = DnsHdrData; 
    
    //Add the questions if this is not MDNS
    if (dnsProtocol == DNS_PROTOCOL_MDNS)
    {
        DnsHdrQdcount = 0;
        DnsHdrNscount = 0;
        DnsHdrArcount = 0;
    }
    else
    {
        DnsHdrQdcount = qdcount;
        DnsHdrNscount = nscount;
        DnsHdrArcount = arcount;
        while (pRx < pEndRx) *p++ = *pRx++;
    }
    

    if (addAnswers(dnsProtocol)) return DO_NOTHING;
    
    DnsHdrIsReply          = true;
    DnsHdrIsAuthoritative  = false;
    if (dnsProtocol == DNS_PROTOCOL_MDNS) DnsHdrIsAuthoritative  = true; //See rfc6762 18.4
    DnsHdrIsRecursiveQuery = false;
    
    DnsHdrWrite();
    
    *pSizeTx = p - DnsHdrPacket;
    
    if (DnsServerTrace || NetTraceHostGetMatched()) DnsHdrLog(dnsProtocol);

    int dest;
    if (dnsProtocol == DNS_PROTOCOL_MDNS && !mdnsUnicastReply) dest = MULTICAST_MDNS;
    else                                                       dest =   UNICAST;
    
    return ActionMakeFromDestAndTrace(dest, (NetTraceStack && DnsServerTrace) || NetTraceHostGetMatched());
}
