#include <stdbool.h>

#include "lpc1768/mstimer/mstimer.h"
#include "lpc1768/gpio.h"

#define BLINK_DURATION_MS 50

static volatile unsigned* _linkLedDirPtr = 0;
static volatile unsigned* _linkLedSetPtr = 0;
static volatile unsigned* _linkLedClrPtr = 0;
static volatile unsigned* _speedLedDirPtr = 0;
static volatile unsigned* _speedLedSetPtr = 0;
static volatile unsigned* _speedLedClrPtr = 0;

void JackLeds(bool phyLink, bool phySpeed, bool activity)
{
    static int blinkTimer = 0;
	
	if (!_linkLedDirPtr) return;
	if (!_linkLedSetPtr) return;
	if (!_linkLedClrPtr) return;
	if (!_speedLedDirPtr) return;
	if (!_speedLedSetPtr) return;
	if (!_speedLedClrPtr) return;
	
	*_linkLedDirPtr  = 1; //Set the direction to 1 == output
	*_speedLedDirPtr = 1; //Set the direction to 1 == output
    
    if (activity) blinkTimer = MsTimerCount;
    if (MsTimerRelative(blinkTimer, BLINK_DURATION_MS))
    {
        if (phyLink ) *_linkLedSetPtr  = 1; else *_linkLedClrPtr  = 1;
        if (phySpeed) *_speedLedSetPtr = 1; else *_speedLedClrPtr = 1;
    }
    else
    {
        *_linkLedClrPtr  = 1;
        *_speedLedClrPtr = 1;
    }
}
void JackInit(char* linkLedPin, char* speedLedPin)
{
	_linkLedDirPtr = GpioDirPtr(linkLedPin);
	_linkLedSetPtr = GpioSetPtr(linkLedPin);
	_linkLedClrPtr = GpioClrPtr(linkLedPin);
	_speedLedDirPtr = GpioDirPtr(speedLedPin);
	_speedLedSetPtr = GpioSetPtr(speedLedPin);
	_speedLedClrPtr = GpioClrPtr(speedLedPin);
}