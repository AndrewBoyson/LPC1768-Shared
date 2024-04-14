#include <stdbool.h>

#include "lpc1768/gpio.h"
#include "lpc1768/led.h"
#include "log/log.h"
#include "net/action.h"
#include "net/eth/eth.h"
#include "net/eth/mac.h"
#include "net/net.h"
#include "nic.h"
#include "jack.h"
#include "link.h"

#define  LINK_PIN FIO1PIN(25)
#define SPEED_PIN FIO1PIN(26)

bool LinkTrace = false;

void LinkMain()
{    
    //Wait until the network is up
    if (!NicLinkIsUp()) return;
    
    //Reset the trace of host at the very start
    NetTraceHostResetMatched();
    
    //Handle packets
    int sizeRx;
    int sizeTx;
    char* pRx = NicGetReceivedPacketOrNull(&sizeRx);
    char* pTx = NicGetTransmitPacketOrNull(&sizeTx);
    
    int  action   = DO_NOTHING;
    bool activity = false;
    if (pRx)
    {
        activity = true;
        
        if (pTx) action = EthHandlePacket(pRx, sizeRx, pTx, &sizeTx);
        
        NicReleaseReceivedPacket();
    }
    
    if (pTx)
    {
        if (!action) action = EthPollForPacketToSend(pTx, &sizeTx);
    
        if ( action)
        {
            activity = true;
            NicSendTransmitPacket(sizeTx);
        }
    }
    
    //Flash lights
    JackLeds(!LINK_PIN, !SPEED_PIN, activity);
}

void LinkInit()
{
    NicInit();
    NicLinkAddress(MacLocal);
    if (LinkTrace)
    {
        LogTime("MAC ");
        MacLog(MacLocal);
        Log("\r\n");
    }
}