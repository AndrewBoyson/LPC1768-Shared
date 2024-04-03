#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "log/log.h"
#include "lpc1768/mstimer/mstimer.h"
#include "net/net.h"
#include "net-this/net-this.h"
#include "net/action.h"
#include "net/eth/eth.h"
#include "net/ip4/ip4addr.h"
#include "net/eth/mac.h"
#include "net/udp/udp.h"
#include "net/udp/dns/dnslabel.h"
#include "dhcphdr.h"

bool DhcpTrace = false;

#define REQUEST   1
#define REPLY     2

#define DHCPDISCOVER 1
#define DHCPOFFER    2
#define DHCPREQUEST  3
#define DHCPDECLINE  4
#define DHCPACK      5
#define DHCPNAK      6
#define DHCPRELEASE  7
#define DHCPINFORM   8

#define COOKIE 0x63825363

#define MAX_REPEAT_DELAY_TIME_MS 64000
#define MIN_REPEAT_DELAY_TIME_MS  4000

#define STATE_NONE     0
#define STATE_DISCOVER 1
#define STATE_SELECT   2
#define STATE_BOUND    3
#define STATE_RENEW    4
#define STATE_REBIND   5
#define STATE_EXPIRED  6

static uint32_t       _xid = 0;
static int          _state = STATE_NONE;
static uint32_t _offeredIp = 0;
static uint32_t  _serverId = 0;
static bool   _awaitingResponse = false;

static uint32_t _elapsedLifeMsTimer  = 0;                       //Started whenever an IP address request has been acknowledged

uint32_t DhcpGetElapsedLife()
{
    if (!_elapsedLifeMsTimer) return 0;
    return (MsTimerCount - _elapsedLifeMsTimer) / 1000;
}

static uint8_t  _dhcpMessageType = 0;
uint32_t DhcpLeaseTime   = 0;
uint32_t DhcpServerIp    = 0;
uint32_t DhcpRouterIp    = 0;
uint32_t DhcpSubnetMask  = 0;
uint32_t DhcpNtpIp       = 0;
uint32_t DhcpRenewalT1   = 0;
uint32_t DhcpRenewalT2   = 0;
uint32_t DhcpBroadcastIp = 0;
uint32_t DhcpLocalIp     = 0;
uint32_t DhcpDnsServerIp = 0;
char     DhcpDomainName[DNS_MAX_LABEL_LENGTH+1];
char     DhcpHostName  [DNS_MAX_LABEL_LENGTH+1];

static void clearAll()
{
    DhcpLeaseTime     = 0;
    DhcpServerIp      = 0;
    DhcpRouterIp      = 0;
    DhcpSubnetMask    = 0;
    DhcpNtpIp         = 0;
    DhcpRenewalT1     = 0;
    DhcpRenewalT2     = 0;
    DhcpBroadcastIp   = 0;
    DhcpLocalIp       = 0;
    DhcpDnsServerIp   = 0;
    DhcpDomainName[0] = 0;
    DhcpHostName  [0] = 0;
    _offeredIp        = 0;
    _serverId         = 0;
    _awaitingResponse = false;
}

bool DhcpIpNeedsToBeRouted(uint32_t ip)
{   
    if ((ip & 0x000000FF) == 0xFF) return false; //Broadcast 255.xxx.xxx.xxx
    if ((ip & 0x000000FF) == 0xE0) return false; //Multicast 224.xxx.xxx.xxx
    if (ip == (DhcpLocalIp | 0xFF000000)) return false; //Local broadcast ip == 192.168.0.255
    
    return (ip & DhcpSubnetMask) != (DhcpLocalIp & DhcpSubnetMask); //ip != 192.168.0.xxx
}

static uint32_t readOption32(char** pp)
{
    uint32_t value = 0;
    char* p = *pp;
    int len = *++p;
    if (len >= 4)
    {
        value  = *++p << 24;
        value |= *++p << 16;
        value |= *++p <<  8;
        value |= *++p <<  0;
    }
    *pp += len + 1;
    return value;
}
static uint32_t readIp(char** pp)
{
    uint32_t value = 0;
    char* p = *pp;
    int len = *++p;
    if (len >= 4)
    {
        value  = *++p <<  0;
        value |= *++p <<  8;
        value |= *++p << 16;
        value |= *++p << 24;
    }
    *pp += len + 1;
    return value;
}
static void readString(char** pp, char* pText)
{
    char* p = *pp;
    int len = *++p;
    for (int i = 0; i < len; i++) pText[i] = *++p;
    *pp += len + 1;
}
static void readOptions(int size, char * pOptions)
{
    int len = 0;
    char* p  = pOptions;
    char* pE = pOptions + size;
    while( p < pE)
    {
        switch (*p)
        {
            case 0:                                                      break;  //NOP
            case 255:                                                    return; //End of options
            case 1:                DhcpSubnetMask  = readIp(&p);         break;  //Subnet Mask
            case 3:                DhcpRouterIp    = readIp(&p);         break;  //Router
            case 6:                DhcpDnsServerIp = readIp(&p);         break;  //DNS server
            case 12:               readString(&p, DhcpHostName);         break;  //Host name
            case 15:               readString(&p, DhcpDomainName);       break;  //Domain name
            case 19:  len = *++p; p+= len;                               break;  //IP forwarding yes/no
            case 28:               DhcpBroadcastIp = readIp(&p);         break;  //Broadcast IP
            case 42:               DhcpNtpIp       = readIp(&p);         break;  //NTP
            case 44:  len = *++p; p+= len;                               break;  //NetBIOS name server
            case 45:  len = *++p; p+= len;                               break;  //NetBIOS datagram server
            case 46:  len = *++p; p+= len;                               break;  //NetBIOS node type
            case 47:  len = *++p; p+= len;                               break;  //NetBIOS scope
            case 53:  len = *++p; _dhcpMessageType = *++p;               break;  //DHCP message type
            case 51:               DhcpLeaseTime   = readOption32(&p);   break;  //Address lease time
            case 54:               _serverId       = readIp(&p);         break;  //DHCP server
            case 58:               DhcpRenewalT1   = readOption32(&p);   break;  //T1
            case 59:               DhcpRenewalT2   = readOption32(&p);   break;  //T2
            default:
                if (DhcpTrace) LogTimeF("Ignoring option %d\r\n", *p);
                len = *++p;
                p += len;
                return;
        }
        p++;
    }
}
static void readOptionMessageType(int size, char * pOptions)
{
    int len = 0;
    char* p  = pOptions;
    char* pE = pOptions + size;
    while( p < pE)
    {
        switch (*p)
        {
            case 0:                                                      break;  //NOP
            case 255:                                                    return; //End of options
            case 53:  len = *++p; _dhcpMessageType = *++p;               break;  //DHCP message type
            default:  len = *++p; p += len;                              break;
        }
        p++;
    }
}
static void readOptionServerId(int size, char * pOptions)
{
    int len = 0;
    char* p  = pOptions;
    char* pE = pOptions + size;
    while( p < pE)
    {
        switch (*p)
        {
            case 0:                                                      break;  //NOP
            case 255:                                                    return; //End of options
            case 54:               _serverId       = readIp(&p);         break;  //DHCP server
            default:  len = *++p; p += len;                              break;
        }
        p++;
    }
}
static void writeIp(uint8_t code, uint32_t value, char** pp)
{
    if (!value) return;

    char* p = *pp;

    *p++ = code;
    *p++ = 4;
    *p++ = (value & 0x000000FF) >>  0;
    *p++ = (value & 0x0000FF00) >>  8;
    *p++ = (value & 0x00FF0000) >> 16;
    *p++ = (value & 0xFF000000) >> 24;

    *pp += 6;
}
static void writeText(uint8_t code, const char* pText, char** pp)
{
    if (!pText) return;
    if (!*pText) return;
    
    char* p = *pp;
    
    *p++ = code;
    char* pLength = p++;
    while (true) //Copy pText without the end zero
    {
        *p = *pText;
        pText++;
        if (!*pText) break;
        p++;
    }
    
    *pLength = p - pLength;
    
    *pp += *pLength + 2;
}
int makeRequest(void* pPacket, uint8_t type, uint32_t ciaddr, uint32_t requestedIp, uint32_t serverId)
{
    bool broadcast = ciaddr == 0;
    uint16_t flags = 0;
    if (broadcast) flags |= 0x8000;
    DhcpHdrSetOp    (pPacket, REQUEST            );
    DhcpHdrSetHtype (pPacket, ETHERNET           );
    DhcpHdrSetHlen  (pPacket, 6                  );
    DhcpHdrSetHops  (pPacket, 0                  );
    DhcpHdrSetXid   (pPacket, _xid               ); //Randomly chosed transaction id used to associate messages to responses
    DhcpHdrSetSecs  (pPacket, 0                  ); //Seconds since started to boot
    DhcpHdrSetFlags (pPacket, flags              ); //Broadcast (1) Unicast (0)
    DhcpHdrSetCiaddr(pPacket, ciaddr             ); //'Client' address set by client or 0 if don't know address
    DhcpHdrSetYiaddr(pPacket, 0                  ); //'Your' address returned by server
    DhcpHdrSetSiaddr(pPacket, 0                  ); //'Server' address to use if required
    DhcpHdrSetGiaddr(pPacket, 0                  ); //'Gateway' address
    memcpy(DhcpHdrPtrChaddr(pPacket), MacLocal, 6); //'Client hardware' address. 6 bytes for ethernet
    memset(DhcpHdrPtrChaddr(pPacket) + 6, 0, 10  ); //Pad the remainder of the hardware address field with zeros
    memset(DhcpHdrPtrLegacy(pPacket), 0, 192     ); //BootP legacy fill with zeros
    DhcpHdrSetCookie(pPacket, COOKIE             ); //Magic cookie

    char* pOptions = (char*)pPacket + DHCP_HEADER_LENGTH;
    char* p = pOptions;
    *p++ = 53; *p++ = 1; *p++ = type;                  //DHCP message type
    if (requestedIp) writeIp(50, requestedIp, &p);     //Requested IP
    if (   serverId) writeIp(54,    serverId, &p);     //Server ip
    writeText(12, NET_NAME, &p);                       //Host name
    *p++ = 255;                                        //End of options
    
    return DHCP_HEADER_LENGTH + p - pOptions;
}
int DhcpHandleResponse(void (*traceback)(void), int sizeRx, char* pPacketRx, int* pSizeTx, char* pPacketTx)
{
    uint8_t  op      = DhcpHdrGetOp    (pPacketRx);
    uint8_t  htype   = DhcpHdrGetHtype (pPacketRx);
    uint8_t  hlen    = DhcpHdrGetHlen  (pPacketRx);
    uint32_t xid     = DhcpHdrGetXid   (pPacketRx);    //Randomly chosen transaction id used to associate messages to responses
    uint32_t yiaddr  = DhcpHdrGetYiaddr(pPacketRx);
    //uint32_t siaddr  = DhcpHdrGetSiaddr(pPacketRx);
    uint32_t cookie  = DhcpHdrGetCookie(pPacketRx);

    if (op     != REPLY)                                  return DO_NOTHING;
    if (htype  != ETHERNET)                               return DO_NOTHING;
    if (hlen   != 6)                                      return DO_NOTHING;
    if (memcmp(DhcpHdrPtrChaddr(pPacketRx), MacLocal, 6)) return DO_NOTHING;
    if (!_xid)                                            return DO_NOTHING;
    if (xid    != _xid)                                   return DO_NOTHING;
    if (cookie != COOKIE)                                 return DO_NOTHING;

    char* pOptions = (char*)pPacketRx + DHCP_HEADER_LENGTH;
    readOptionMessageType(sizeRx - DHCP_HEADER_LENGTH, pOptions);
    
    switch (_dhcpMessageType)
    {
        case DHCPOFFER:
            if (_state == STATE_DISCOVER)
            {
                _offeredIp = yiaddr;
                readOptionServerId(sizeRx - DHCP_HEADER_LENGTH, pOptions);
                if (DhcpTrace) { LogTime("DHCP <- offer  "); Ip4AddrLog(_offeredIp); Log(" from server "); Ip4AddrLog(_serverId); Log("\r\n"); }
                _awaitingResponse = false;
            }
            else
            {
                if (DhcpTrace) { LogTime("DHCP <- offer  "); Ip4AddrLog(_offeredIp); Log(" from server "); Ip4AddrLog(_serverId); Log(" ignored\r\n"); }
            }
            break;
        case DHCPACK:
            if (_state == STATE_SELECT || _state == STATE_RENEW || _state == STATE_REBIND)
            {
                DhcpLocalIp = yiaddr;
                readOptions(sizeRx - DHCP_HEADER_LENGTH, pOptions);
                DhcpServerIp = _serverId;
                if (DhcpTrace) { LogTime("DHCP <- ack    ");   Ip4AddrLog(DhcpLocalIp); Log(" from server "); Ip4AddrLog(_serverId); Log("\r\n"); }
                _elapsedLifeMsTimer = MsTimerCount;  //Start the life timer
                _awaitingResponse = false;
            }
            else
            {
                if (DhcpTrace) { LogTime("DHCP <- ack    ");   Ip4AddrLog(DhcpLocalIp); Log(" from server "); Ip4AddrLog(_serverId); Log(" ignored\r\n"); }
            }
            break;
        case DHCPNAK:
            if (_state == STATE_SELECT || _state == STATE_RENEW || _state == STATE_REBIND)
            {
                readOptionServerId(sizeRx - DHCP_HEADER_LENGTH, pOptions);
                if (DhcpTrace) { LogTime("DHCP <- nack   ");  Ip4AddrLog(yiaddr); Log(" from server "); Ip4AddrLog(_serverId); Log("\r\n"); }
                clearAll();
                _awaitingResponse = false;
            }
            else
            {
                if (DhcpTrace) { LogTime("DHCP <- nack   ");  Ip4AddrLog(yiaddr); Log(" from server "); Ip4AddrLog(_serverId); Log(" ignored\r\n"); }
            }
            break;
        default:
            LogTimeF("DHCP <- unknown message %d\r\n", _dhcpMessageType);
            break;
    }
    return DO_NOTHING;
}

int DhcpPollForRequestToSend(void* pPacket, int* pSize)
{
    
    if (!_xid)
    {
        _xid  = (uint32_t)MacLocal[2] << 24;
        _xid += (uint32_t)MacLocal[3] << 16;
        _xid += (uint32_t)MacLocal[4] <<  8;
        _xid += (uint32_t)MacLocal[5] <<  0;
    }
    
    //Check if time to update
    uint32_t elapsedTimeMs = MsTimerCount - _elapsedLifeMsTimer;
    uint32_t   leaseTimeMs = DhcpLeaseTime * 1000;
    
    uint32_t T1 =  leaseTimeMs >> 1;                                            //0.5
    uint32_t T2 = (leaseTimeMs >> 1) + (leaseTimeMs >> 2) + (leaseTimeMs >> 3); //0.875 = 0.5 + 0.25 + 0.125
    
    if (!DhcpLocalIp)
    {
        if (!_offeredIp) _state = STATE_DISCOVER;
        else             _state = STATE_SELECT;
    }
    else
    {
        if (elapsedTimeMs < T1)
        {
            _state = STATE_BOUND;
        }
        else if (elapsedTimeMs < T2)
        {
            _state = STATE_RENEW; 
        }
        else if (elapsedTimeMs < leaseTimeMs)
        {
            _state = STATE_REBIND;
        }
        else
        {
            _state = STATE_EXPIRED;
        }
    }
    static int stateLastScan = STATE_NONE;
    bool stateHasChanged = _state != stateLastScan;
    stateLastScan = _state;
    
    static uint32_t delayMs;
    static uint32_t repeatDelayMsTimer;
    
    bool responseTimeout = _awaitingResponse && MsTimerRelative(repeatDelayMsTimer, delayMs);

    bool send = stateHasChanged || responseTimeout;
    
    if (!send) return DO_NOTHING;

    int             dest = DO_NOTHING;
    uint8_t         type = 0; //DISCOVER or REQUEST
    uint32_t      ciaddr = 0; //goes in ciaddr only for BOUND/RENEW/REBIND
    uint32_t requestedIp = 0; //goes in option 54 only for SELECTING
    uint32_t    serverId = 0; //goes in option 50 and is the server id for the accepted offer

    //Send the renewal request
    *pSize = 0;


    switch (_state)
    {
        case STATE_NONE:
            if (DhcpTrace) { LogTimeF("DHCP -- none\r\n"); }
            clearAll();
            return DO_NOTHING;
            
        case STATE_DISCOVER:
            if (DhcpTrace) { LogTimeF("DHCP -> discover\r\n"); }
            type = DHCPDISCOVER;
            ciaddr = 0;
            requestedIp = 0;
            serverId = 0;
            dest = BROADCAST;
            break;
            
        case STATE_SELECT:
            if (DhcpTrace) { LogTimeF("DHCP -> select "); Ip4AddrLog(_offeredIp); Log(" from server "); Ip4AddrLog(_serverId); Log("\r\n"); }
            type = DHCPREQUEST;
            ciaddr = 0;
            requestedIp = _offeredIp;
            serverId = _serverId;
            dest = BROADCAST;
            break;
            
        case STATE_BOUND:
            if (DhcpTrace) { LogTimeF("DHCP -- bound  "); Ip4AddrLog(DhcpLocalIp); Log("\r\n"); }
            return DO_NOTHING;
            
        case STATE_RENEW:
            if (DhcpTrace) { LogTimeF("DHCP -> renew (T1) "); Ip4AddrLog(DhcpLocalIp); Log("\r\n"); }
            type = DHCPREQUEST;
            ciaddr = DhcpLocalIp;
            requestedIp = 0;
            serverId = 0;
            dest = UNICAST_DHCP;
            break;
            
        case STATE_REBIND:
            if (DhcpTrace) { LogTimeF("DHCP -> rebind (T2) "); Ip4AddrLog(DhcpLocalIp); Log("\r\n"); }
            type = DHCPREQUEST;
            ciaddr = DhcpLocalIp;
            requestedIp = 0;
            serverId = 0;
            dest = BROADCAST;
            break;
            
        case STATE_EXPIRED:
            if (DhcpTrace) { LogTimeF("DHCP -- expired "); Ip4AddrLog(DhcpLocalIp); Log("\r\n"); }
            clearAll();
            return DO_NOTHING;
            
        default:
            LogTimeF("DHCP -- unknown state %d\r\n", _state);
            return 0;
    }
    
    if (!_awaitingResponse)
    {
        delayMs = MIN_REPEAT_DELAY_TIME_MS;
    }
    else
    {
        delayMs <<= 1;                                                                //Backoff (double) the delay time after each attempt
        if (delayMs > MAX_REPEAT_DELAY_TIME_MS) delayMs = MAX_REPEAT_DELAY_TIME_MS;   //Don't go beyond a maximum
    }
    _awaitingResponse = true;
    repeatDelayMsTimer = MsTimerCount;                                                //Start the repeat delay timer
    *pSize = makeRequest(pPacket, type, ciaddr, requestedIp, serverId);
    
    return ActionMakeFromDestAndTrace(dest, DhcpTrace && NetTraceStack);
}