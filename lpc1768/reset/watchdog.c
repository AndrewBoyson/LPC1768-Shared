#include "lpc1768/register.h"

void WatchdogInit()
{
    //Set up the watchdog timer to interrupt if not fed every two seconds
    WDCLKSEL = 0x2;     //Selects the RTC oscillator (rtc_clk) as the Watchdog clock source.
    WDTC     = 0x4000;  //Counts at 4/32khz therefore 1sec = 0x2000, 2sec = 0x4000, 16sec = 0x20000, 64sec = 0x80000.
    WDMOD    = 0x1;     //Watchdog enable WDEN(bit0 = 1); reset enable (no interrupt) WDRESET(bit1 = 0); clear WDTOF(bit2 = 0)
    WDFEED   = 0xAA;    //Start the watchdog
    WDFEED   = 0x55;
    ISER0   |= 1U << 0; //6.5.1 bit0 == Interrupt set enable for WDT.
}
void WatchdogMain()
{
    WDFEED = 0xAA;     //Feed the watchdog
    WDFEED = 0x55;
}
