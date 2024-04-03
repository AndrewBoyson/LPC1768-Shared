#include <stdint.h>

#include "log/log.h"
#include "tcp.h"
#include "tcb.h"
#include "web/http/http.h"
#include "lpc1768/mstimer/mstimer.h"

#define TCB_COUNT 10

struct tcb tcbs[TCB_COUNT];

uint32_t TcbGetIsn()
{
    static uint32_t isn = 0;
    isn += 100000; //Gives each tcb 100,000 packets and won't repeat before 42,940 tcbs.
    return isn;
}
struct tcb* TcbGetExisting(int ipType, int remArIndex, int locIpScope, uint16_t remPort, uint16_t locPort)
{
    for (int i = 0; i < TCB_COUNT; i++)
    {
        struct tcb* pTcb = tcbs + i;
        if (pTcb->state                    && 
            pTcb->ipType     == ipType     &&
            pTcb->remArIndex == remArIndex &&
            pTcb->locIpScope == locIpScope &&
            pTcb->remPort    == remPort    &&
            pTcb->locPort    == locPort) return pTcb;
    }
    return NULL;
}
struct tcb* TcbGetEmpty()
{
    for (int i = 0; i < TCB_COUNT; i++)
    {
        struct tcb* pTcb = tcbs + i;
        if (pTcb->state == TCB_EMPTY) return pTcb;
    }
    return NULL;
}
struct tcb* TcbGetNext(struct tcb* pTcb)
{
    if (!pTcb) return tcbs; //Initialise if passed NULL
    ++pTcb; //Increment
    if (pTcb < tcbs + TCB_COUNT) return pTcb;
    else                         return tcbs;
    
}
void TcbSendAjax()
{
    for (int i = 0; i < TCB_COUNT; i++)
    {
        struct tcb* pTcb = tcbs + i;
        if (pTcb->state)
        {
            HttpAddByteAsHex (pTcb->state);                       HttpAddChar('\t');
            HttpAddInt32AsHex(MsTimerCount - pTcb->timeLastRcvd); HttpAddChar('\t');
            HttpAddInt16AsHex(pTcb->ipType);                      HttpAddChar('\t');
            HttpAddInt32AsHex(pTcb->remArIndex);                  HttpAddChar('\t');
            HttpAddInt16AsHex(pTcb->locPort);                     HttpAddChar('\t');
            HttpAddInt16AsHex(pTcb->remPort);                     HttpAddChar('\t');
            HttpAddInt32AsHex(pTcb->bytesRcvdFromRem);            HttpAddChar('\t');
            HttpAddInt32AsHex(pTcb->bytesSentToRem);              HttpAddChar('\n');
        }
    }
}

int TcbGetId(struct tcb* pTcb) //0 means none
{
    return pTcb - tcbs + 1;
}
void TcbInit()
{
    for (int i = 0; i < TCB_COUNT; i++) tcbs[i].state = TCB_EMPTY;
}
