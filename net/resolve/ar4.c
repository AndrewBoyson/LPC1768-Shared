#include <stdint.h>
#include <stdbool.h>
#include "log/log.h"
#include "net/net.h"
#include "net/eth/mac.h"
#include "net/ip4/ip4addr.h"
#include "net/arp/arp.h"
#include "web/http/http.h"
#include "lpc1768/mstimer/mstimer.h"

bool Ar4Trace = false;

#define  CACHE_TIMEOUT_MS  3600 * 1000
#define FREEZE_TIMEOUT_MS  1800 * 1000
#define  REPLY_TIMEOUT_MS     1 * 1000
#define SEND_ATTEMPTS      3
#define RECORDS_COUNT     30

#define STATE_EMPTY 0
#define STATE_WANT  1
#define STATE_SENT  2
#define STATE_VALID 3

struct record
{
    uint32_t elapsed;
    uint32_t ip;
    uint8_t  state;
    uint8_t  tries;
    char     mac[6];
};
static struct record records[RECORDS_COUNT];
static int getExistingIp(uint32_t ip)
{
    for (int i = 0; i < RECORDS_COUNT; i++)
    {
        if (records[i].state && records[i].ip == ip) return i;
    }
    return -1;
}
static int getOldest()
{
    int        iOldest = 0;
    uint32_t ageOldest = 0;
    for (int i = 0; i < RECORDS_COUNT; i++)
    {
        if (!records[i].state) return i; //Found an empty slot so just return it
        uint32_t age = MsTimerCount - records[i].elapsed;
        if (age >= ageOldest)
        {
            ageOldest = age;
              iOldest = i;
        }
    }  
    return iOldest;                         //Otherwise return the oldest
}
void Ar4MakeRequestForMacFromIp(uint32_t ip)
{
    //Don't treat non ips
    if (!ip) return;
    int i;
    
    //If a record already exists then request an update
    i = getExistingIp(ip);
    if (i > -1)
    {
        if (!MsTimerRelative(records[i].elapsed, FREEZE_TIMEOUT_MS)) return;
        if (Ar4Trace)
        {
            LogTimeF("AR4 Updated request for MAC of ");
            Ip4AddrLog(ip);
            Log("\r\n");
        }
        records[i].state    = STATE_WANT;
        records[i].tries    = 0;
        records[i].elapsed  = MsTimerCount;
        return;
    }
    
    //If a record does not exist then find the first empty slot and add the IP and date
    if (Ar4Trace)
    {
        LogTimeF("AR4 Made request for MAC of ");
        Ip4AddrLog(ip);
        Log("\r\n");
    }
    i = getOldest();
    records[i].ip       = ip;
    records[i].state    = STATE_WANT;
    records[i].tries    = 0;
    records[i].elapsed  = MsTimerCount;
    MacClear(records[i].mac);
}
int Ar4AddIpRecord(void (*traceback)(void), char* mac, uint32_t ip)
{
    //Don't treat non ips
    if (!ip)
    {
        if (Ar4Trace)
        {
            LogTime("Ar4AddIpRecord had blank ip\r\n");
            if (NetTraceStack) traceback();
        }
        return -1;
    }
    if (MacIsEmpty(mac))
    {
        if (Ar4Trace)
        {
            LogTime("Ar4AddIpRecord had blank mac\r\n");
            if (NetTraceStack) traceback();
        }
        return -1;
    }
    int i;
    
    //See if any record corresponds to the IP and, if so, update the MAC and time
    i = getExistingIp(ip);
    if (i > -1)
    {
        records[i].elapsed = MsTimerCount;
        records[i].state   = STATE_VALID;
        MacCopy(records[i].mac, mac);
        return i;
    }
    
    //Otherwise find the first empty slot and add the IP, MAC, and date
    i = getOldest();
    records[i].ip       = ip;
    records[i].elapsed  = MsTimerCount;
    records[i].state = STATE_VALID;
    MacCopy(records[i].mac, mac);
    return i;
}
void Ar4IpToMac(uint32_t ip, char* mac)
{
    for (int i = 0; i < RECORDS_COUNT; i++)
    {
        if (records[i].state == STATE_VALID && records[i].ip == ip)
        {
            MacCopy(mac, records[i].mac);
            return;
        }
    }
    MacClear(mac);
}
uint32_t Ar4GetIpFromMac(char* pMac)
{
    for (int i = 0; i < RECORDS_COUNT; i++)
    {
        if (records[i].state == STATE_VALID && MacIsSame(records[i].mac, pMac)) return records[i].ip;
    }
    return 0;
}
bool Ar4HaveMacForIp(uint32_t ip)
{
    for (int i = 0; i < RECORDS_COUNT; i++)
    {
        if (records[i].state == STATE_VALID && records[i].ip == ip) return true;
    }
    return false;
}
bool Ar4CheckHaveMacAndFetchIfNot(uint32_t ip)
{
    if (!Ar4HaveMacForIp(ip))
    {
        Ar4MakeRequestForMacFromIp(ip); //The request is only repeated if made after a freeze time - call as often as you want.
        return false;
    }
    return true;
}
uint32_t Ar4IndexToIp(int i)
{
    return records[i].ip;
}
void Ar4SendHttp()
{
    for (int i = 0; i < RECORDS_COUNT; i++)
    {
        if (records[i].state)
        {
            HttpAddF("%4u ", (MsTimerCount - records[i].elapsed) / 1000 / 60);
            
            int ipLen = Ip4AddrHttp(records[i].ip);
            HttpAddFillChar(' ', 40 - ipLen);
            
            MacHttp(records[i].mac);
            
            HttpAddChar('\r');
            HttpAddChar('\n');
        }
    }
}
void Ar4SendAjax()
{
    for (int i = 0; i < RECORDS_COUNT; i++)
    {
        if (records[i].state)
        {
            HttpAddByteAsHex(i);
            HttpAddChar('\t');
            HttpAddInt32AsHex(MsTimerCount - records[i].elapsed);
            HttpAddChar('\t');
            HttpAddInt32AsHex(records[i].ip);
            HttpAddChar('\t');
            for (int b = 0; b < 6; b++) HttpAddByteAsHex(records[i].mac[b]);
            HttpAddChar('\n');
        }
    }
}
static void clear(struct record* pr)
{
    pr->state = STATE_EMPTY;
}
static void clearCache(struct record* pr)
{
    if (MsTimerRelative(pr->elapsed, CACHE_TIMEOUT_MS)) clear(pr);
}
static void retry(struct record* pr)
{
    if (pr->state == STATE_SENT && MsTimerRelative(pr->elapsed, REPLY_TIMEOUT_MS))
    {
        if (pr->tries < SEND_ATTEMPTS)
        {
            pr->state   = STATE_WANT;
            pr->elapsed = MsTimerCount;
            pr->tries++;
        }
        else
        {
            clear(pr);
        }
    }
}
static void sendRequest(struct record* pr)
{
    if (!ArpResolveRequestFlag)
    {
        if (pr->state == STATE_WANT)
        {
            if (Ar4Trace)
            {
                LogTimeF("AR4 Send request for MAC from IP4 ");
                Ip4AddrLog(pr->ip);
                Log("\r\n");
            }
            ArpAddressToResolve = pr->ip;
            ArpResolveRequestFlag = true;
            pr->state   = STATE_SENT;
            pr->elapsed = MsTimerCount;
            return;
        }
    }
}
void Ar4Main()
{
    static int i = -1;
    i++;
    if (i >= RECORDS_COUNT) i = 0;
    
    struct record* pr = &records[i];
    
    clearCache (pr);
    retry      (pr);
    sendRequest(pr);
}
void Ar4Init()
{
    for (int i = 0; i < RECORDS_COUNT; i++)
    {
        struct record* pr = &records[i];
        clear(pr);
    }
}
