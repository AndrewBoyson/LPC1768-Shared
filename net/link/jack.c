#include <stdbool.h>

#include "lpc1768/mstimer/mstimer.h"

#define BLINK_DURATION_MS 50

extern void NetThisLinkLed (char on);
extern void NetThisSpeedLed(char on);

void JackLeds(bool phyLink, bool phySpeed, bool activity)
{
    static int blinkTimer = 0;
    
    if (activity) blinkTimer = MsTimerCount;
    if (MsTimerRelative(blinkTimer, BLINK_DURATION_MS))
    {
        if (phyLink )  NetThisLinkLed(1); else  NetThisLinkLed(0);
        if (phySpeed) NetThisSpeedLed(1); else NetThisSpeedLed(0);
    }
    else
    {
         NetThisLinkLed(0);
        NetThisSpeedLed(0);
    }
}