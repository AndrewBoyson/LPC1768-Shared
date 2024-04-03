#include <stdint.h>
#include <stdbool.h>

#include "log/log.h"
#include "net/net.h"
#include "ndp.h"
#include "net/ip6/ip6.h"
#include "net/ip6/ip6addr.h"
#include "net/ip6/slaac.h"
#include "net/action.h"
#include "net/eth/mac.h"
#include "lpc1768/mstimer/mstimer.h"

bool RsTrace = false;

bool RsSendSolicitation = false;

#define MAX_REPEAT_DELAY_TIME_MS 60000
#define MIN_REPEAT_DELAY_TIME_MS   800
static uint32_t repeatDelayMsTimer = (uint32_t)-MIN_REPEAT_DELAY_TIME_MS; //Initial value ensures no delay at startup
static uint32_t delayMs            =            MIN_REPEAT_DELAY_TIME_MS; //Doubles on failure up to max; reset to min whenever an IP address request has been acknowledged

static void hdrSetReserved(char* pPacket, uint32_t value) { NetInvert32(pPacket + 0, &value); }
static const int HEADER_LENGTH = 4;

static void logHeader(char* pPacket, int size)
{
    char* pData = pPacket + HEADER_LENGTH;
    int dataLength = size - HEADER_LENGTH;
    
    if (NetTraceVerbose)
    {
        Log("RS header\r\n");
        LogF("  Size        %d\r\n", size);
        NdpLogOptionsVerbose(pData, dataLength);
    }
    else
    {
        Log("RS    header");
        NdpLogOptionsQuiet(pData, dataLength);
        Log("\r\n");
    }
}
int RsGetWaitingSolicitation(char* pPacket, int* pSize, uint8_t* pType, uint8_t* pCode)
{
    //Check if time to update
    if (NdpIsFresh())
    {
        delayMs = MIN_REPEAT_DELAY_TIME_MS; //Set the delay time back to minimum
        return DO_NOTHING;
    }

    //Limit retries with a backoff delay
    if (!MsTimerRelative(repeatDelayMsTimer, delayMs)) return DO_NOTHING;          //Don't retry within the delay time
    delayMs <<= 1;                                                                 //Backoff (double) the delay time after each attempt
    if (delayMs > MAX_REPEAT_DELAY_TIME_MS) delayMs = MAX_REPEAT_DELAY_TIME_MS;    //Don't go beyond a maximum
    repeatDelayMsTimer = MsTimerCount;                                             //Start the delay timer

    //Send the renewal request
    *pType = 133; //Router solicitation
    *pCode = 0;

    hdrSetReserved(pPacket, 0);
    
    char* pData = (char*)pPacket + HEADER_LENGTH;
    char* p = pData;
    p += NdpAddOptionSourceMac(p, MacLocal);
    
    *pSize = HEADER_LENGTH + p - pData;
    
    if (RsTrace)
    {
        if (NetTraceNewLine) Log("\r\n");
        LogTime("NDP send router solicit\r\n");
        logHeader(pPacket, *pSize);
    }
    
    return ActionMakeFromDestAndTrace(MULTICAST_ROUTER, RsTrace && NetTraceStack);
    
}
