#include  <stdint.h>
#include <stdbool.h>

#include "log/log.h"
#include "net/net.h"
#include "net/action.h"

bool Dest6Trace = false;

static void logCode(uint8_t code)
{
    switch (code)
    {
        case 0:  Log ("No route to destination"                                   ); break;
        case 1:  Log ("Communication with destination administratively prohibited"); break;
        case 2:  Log ("Beyond scope of source address"                            ); break;
        case 3:  Log ("Address unreachable"                                       ); break;
        case 4:  Log ("Port unreachable"                                          ); break;
        case 5:  Log ("Source address failed ingress/egress policy"               ); break;
        case 6:  Log ("Reject route to destination"                               ); break;
        default: LogF("Unknown code %u", code                                     ); break;
    }
}
int Dest6HandleRequest(void (*traceback)(void), uint8_t* pType, uint8_t* pCode)
{
    if (Dest6Trace)
    {
        if (NetTraceNewLine) Log("\r\n");
        LogTime("Dest6 - Destination unreacheable - "); logCode(*pCode); Log("\r\n");
        if (NetTraceStack) traceback();
    }
    return DO_NOTHING;
}