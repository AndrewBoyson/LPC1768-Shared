#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "log/log.h"
#include "net/net.h"
#include "net/action.h"

bool Echo4Trace = false;

int Echo4HandleRequest(void (*traceback)(void), uint8_t* pType, uint8_t* pCode, char* payloadRx, int sizeRx, char* payloadTx, int* pSizeTx)
{
    if (Echo4Trace)
    {
        if (NetTraceNewLine) Log("\r\n");
        LogTime("Echo4\r\n");
        if (NetTraceStack) traceback();
    }
    *pType = 0;
    *pCode = 0;
    *pSizeTx = sizeRx;
    memcpy(payloadTx, payloadRx, *pSizeTx);
    return ActionMakeFromDestAndTrace(UNICAST, Echo4Trace && NetTraceStack);
}