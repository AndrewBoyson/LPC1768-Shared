#include "register.h"
#include "bitband.h"

static void periphInit()
{
    PCONP  = 0; //Start with all peripherals off
}
static void flashInit()
{
    FLASHCFG &= ~(0xF << 12); // b15:12 0b0100 Flash accesses use 5 CPU clocks. Use for up to 100 MHz CPU clock.
    FLASHCFG |=   0x4 << 12 ;
}
static void oscInit()
{
    //Enable the main oscillator and wait for it to be ready
    OSCRANGE = 0;                  //Main Oscillator Range Select: 0= 1 MHz to 20 MHz; 1= 15 MHz to 25 MHz
    OSCEN    = 1;                  //Main Oscillator Enable
    while (!OSCSTAT) asm("nop");   //Wait for Main Oscillator to be ready
    
    //Change the CPU clock divider 4.5.13 steps 3 and 7
    CCLKCFG   = 2;                 // bit 7:0 CCLKSEL: Divide value for the CPU clock (CCLK) from the PLL0 output. 2 == divide by 3
    
    //PLL0 Clock Source Select 4.5.13 step 4
    CLKSRCSEL = 1;                 //01 selects the main oscillator; 00 (default) internal RC (4MHz +/-1%); 10 RTC (32KHz)
    
    //PLLO Configuration 4.5.13 step 5
    PLL0CFG   = 11;                //MSEL0=11 == M=12 == multiply by 24; NSEL=0 == divide by 1
    PLL0FEED  = 0xAA;
    PLL0FEED  = 0x55;
    
    //PLL0 Enable 4.5.13 step 6
    PLLE0     = 1;                 // PLL0 Enable
    PLL0FEED  = 0xAA;
    PLL0FEED  = 0x55;
	
	//Wait for PLL to be locked 4.5.13 step 8
    while (!PLOCK0) asm("nop");
	
	//PLL0 connect 4.5.13 step 9
    PLLC0     = 1;                 // PLL0 Connect
    PLL0FEED  = 0xAA;
    PLL0FEED  = 0x55;
}
static void usbInit()
{
    USBCLKCFG = 5;                 // 5 == divide by 6
}
static void clkOutInit()
{
    CLKOUTCFG = 0;                 //No clock output
}

void SystemInit()
{
	periphInit();
    flashInit();
	oscInit();
    usbInit();
	clkOutInit();
}