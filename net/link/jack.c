#include <stdbool.h>

#include "lpc1768/mstimer/mstimer.h"

#define BLINK_DURATION_MS 50

volatile unsigned* JackLinkLedDirPtr = 0;
volatile unsigned* JackLinkLedSetPtr = 0;
volatile unsigned* JackLinkLedClrPtr = 0;
volatile unsigned* JackSpeedLedDirPtr = 0;
volatile unsigned* JackSpeedLedSetPtr = 0;
volatile unsigned* JackSpeedLedClrPtr = 0;

void JackLeds(bool phyLink, bool phySpeed, bool activity)
{
    static int blinkTimer = 0;
	
	if (!JackLinkLedDirPtr) return;
	if (!JackLinkLedSetPtr) return;
	if (!JackLinkLedClrPtr) return;
	if (!JackSpeedLedDirPtr) return;
	if (!JackSpeedLedSetPtr) return;
	if (!JackSpeedLedClrPtr) return;
	
	*JackLinkLedDirPtr  = 1; //Set the direction to 1 == output
	*JackSpeedLedDirPtr = 1; //Set the direction to 1 == output
    
    if (activity) blinkTimer = MsTimerCount;
    if (MsTimerRelative(blinkTimer, BLINK_DURATION_MS))
    {
        if (phyLink ) *JackLinkLedSetPtr  = 1; else *JackLinkLedClrPtr  = 1;
        if (phySpeed) *JackSpeedLedSetPtr = 1; else *JackSpeedLedClrPtr = 1;
    }
    else
    {
        *JackLinkLedClrPtr  = 1;
        *JackSpeedLedClrPtr = 1;
    }
}