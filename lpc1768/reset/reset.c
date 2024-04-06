#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "rsid.h"
#include "restart.h"
#include "watchdog.h"
#include "lpc1768/led.h"
#include "lpc1768/mstimer/mstimer.h"

bool ResetAccepted = false;

bool ResetWasPushButton()
{
    return RsidExtr && RestartGetLastCause() == RESTART_CAUSE_RESET_BUTTON;
}
bool ResetHasAlarm()
{
    if (ResetAccepted) return false;
    if (RsidExtr)      return false;
    return true;
}
void ResetInit()
{
        RsidInit();
     RestartInit();
    WatchdogInit();
}
void ResetMain()
{
    //Feed the watchdog
    WatchdogMain();
    
    //Flash an led
    static uint32_t ledFlashTimer = 0;
    if (ResetHasAlarm())
    {
        if (MsTimerRepetitive(&ledFlashTimer, 100)) Led4Tgl();
    }
    else
    {
        Led4Set(false);
    }
}
