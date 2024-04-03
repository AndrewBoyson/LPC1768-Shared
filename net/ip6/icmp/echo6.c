#include  <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "log/log.h"
#include "net/net.h"
#include "net/action.h"

bool Echo6Trace = false;

int Echo6HandleRequest(void (*traceback)(void), uint8_t* pType, uint8_t* pCode, char* payloadRx, int sizeRx, char* payloadTx, int* pSizeTx)
{
    if (Echo6Trace)
    {
        if (NetTraceNewLine) Log("\r\n");
        LogTime("Echo6\r\n");
        if (NetTraceStack) traceback();
    }
    *pType = 129;
    *pCode = 0;
    *pSizeTx = sizeRx;
    memcpy(payloadTx, payloadRx, *pSizeTx);
    return ActionMakeFromDestAndTrace(UNICAST, Echo6Trace && NetTraceStack);
}