
#define WDMOD    *((volatile unsigned *) 0x40000000) //Mode
#define WDTC     *((volatile unsigned *) 0x40000004) //Timer constant
#define WDFEED   *((volatile unsigned *) 0x40000008) //Writing 0xAA followed by 0x55 to this register reloads the Watchdog timer with the value contained in WDTC.
#define WDCLKSEL *((volatile unsigned *) 0x40000010) //Clock source selection
#define ISER0    *((volatile unsigned *) 0xE000E100) //Interrupt Set-Enable Register 1

void WatchdogInit()
{
    //Set up the watchdog timer to reset if not fed every second
    WDCLKSEL = 0x2;     //Selects the RTC oscillator (rtc_clk) as the Watchdog clock source.
    WDTC     = 0x2000;  //Counts at 4/32khz therefore 1sec = 0x2000, 16sec = 0x20000 64sec = 0x80000.
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
