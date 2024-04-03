#include <stdbool.h>

#include "lpc1768/gpio.h"
#include "lpc1768/mstimer/mstimer.h"
#include "net-this/net-jack-leds.h"

#define BLINK_DURATION_MS 50

void JackLeds(bool phyLink, bool phySpeed, bool activity)
{
    static int blinkTimer = 0;
    
    if (activity) blinkTimer = MsTimerCount;
    if (MsTimerRelative(blinkTimer, BLINK_DURATION_MS))
    {
        if (phyLink)  LED_GR_L_SET; else LED_GR_L_CLR;
        if (phySpeed) LED_YE_R_SET; else LED_YE_R_CLR;
    }
    else
    {
        LED_GR_L_CLR;
        LED_YE_R_CLR;
    }
}

void JackInit()
{
    LED_GR_L_DIR = 1; //Set the direction to 1 == output
    LED_YE_R_DIR = 1; //Set the direction to 1 == output
}