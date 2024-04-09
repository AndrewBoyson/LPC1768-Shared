#include <stdbool.h>

#include "lpc1768/mstimer/mstimer.h"

#define BLINK_DURATION_MS 50

void (* _linkLed)(char on) = 0; //Initialised from a routine in the application
void (*_speedLed)(char on) = 0; //Initialised from a routine in the application

void JackLeds(bool phyLink, bool phySpeed, bool activity)
{
    static int blinkTimer = 0;
	
	if (! _linkLed) return;
	if (!_speedLed) return;
    
    if (activity) blinkTimer = MsTimerCount;
    if (MsTimerRelative(blinkTimer, BLINK_DURATION_MS))
    {
        if (phyLink )  _linkLed(1); else  _linkLed(0);
        if (phySpeed) _speedLed(1); else _speedLed(0);
    }
    else
    {
         _linkLed(0);
        _speedLed(0);
    }
}

void JackInit(void (* linkLed)(char on), void (*speedLed)(char on))
{
	 _linkLed =  linkLed;
	_speedLed = speedLed;
}