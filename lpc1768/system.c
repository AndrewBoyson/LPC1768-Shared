#include "bitband.h"

//Addresses
#define   FLASHCFG_ADDR 0x400FC000 // Flash Accelerator Configuration Register RW 0x303A
#define    PLL0CON_ADDR 0x400FC080 // PLL0 Control Register                    RW 0
#define    PLL0CFG_ADDR 0x400FC084 // PLL0 Configuration Register              RW 0
#define   PLL0STAT_ADDR 0x400FC088 // PLL0 Status Register                     RO 0
#define   PLL0FEED_ADDR 0x400FC08C // PLL0 Feed Register                       WO NA
#define    CCLKCFG_ADDR 0x400FC104 // CPU Clock Configuration Register         RW 0
#define  USBCLKCFG_ADDR 0x400FC108 // USB Clock Configuration Register         RW 0
#define  CLKSRCSEL_ADDR 0x400FC10C // Clock Source Select Register             RW 0 
#define        SCS_ADDR 0x400FC1A0 // System Control and Status                RW 0
#define  CLKOUTCFG_ADDR 0x400FC1C8 // Clock Output Configuration Register      RW 0

//Registers
#define   FLASHCFG *((volatile unsigned *)  FLASHCFG_ADDR)
#define    CCLKCFG *((volatile unsigned *)   CCLKCFG_ADDR)
#define  CLKSRCSEL *((volatile unsigned *) CLKSRCSEL_ADDR)
#define    PLL0CFG *((volatile unsigned *)   PLL0CFG_ADDR)
#define   PLL0FEED *((volatile unsigned *)  PLL0FEED_ADDR)
#define  USBCLKCFG *((volatile unsigned *) USBCLKCFG_ADDR)
#define  CLKOUTCFG *((volatile unsigned *) CLKOUTCFG_ADDR)

//Bits
#define PLLE0    BIT_BAND4( PLL0CON_ADDR,  0)
#define PLLC0    BIT_BAND4( PLL0CON_ADDR,  1)
#define PLOCK0   BIT_BAND4(PLL0STAT_ADDR, 26) //Reflects the PLL0 Lock status.
#define OSCRANGE BIT_BAND4(     SCS_ADDR,  4)
#define OSCEN    BIT_BAND4(     SCS_ADDR,  5)
#define OSCSTAT  BIT_BAND4(     SCS_ADDR,  6)

void SystemInit()
{
    //Flash config
    FLASHCFG &= ~(0xF << 12); // b15:12 0b0100 Flash accesses use 5 CPU clocks. Use for up to 100 MHz CPU clock.
    FLASHCFG |=   0x4 << 12 ;

    //Enable the main oscillator and wait for it to be ready
    OSCRANGE = 0;                  //Main Oscillator Range Select: 0= 1 MHz to 20 MHz; 1= 15 MHz to 25 MHz
    OSCEN    = 1;                  //Main Oscillator Enable
    while (!OSCSTAT) asm("nop");   //Wait for Main Oscillator to be ready
    
    // bit 7:0 CCLKSEL: Divide value for the CPU clock (CCLK) from the PLL0 output.
    CCLKCFG   = 2;                 // 2 == divide by 3
    
    //PLL0 Clock Source Select
    CLKSRCSEL = 1;                 //01 selects the main oscillator
    
    //PLLO Configuration
    PLL0CFG   = 11;                //MSEL0=11 == M=12 == multiply by 24; NSEL=0 == divide by 1
    PLL0FEED  = 0xAA;
    PLL0FEED  = 0x55;
    
    //PLL0 Control
    PLLE0     = 1;                 // PLL0 Enable
    PLL0FEED  = 0xAA;
    PLL0FEED  = 0x55;
    while (!PLOCK0) asm("nop");    // Wait for PLL to be locked
    PLLC0     = 1;                 // PLL0 Connect
    PLL0FEED  = 0xAA;
    PLL0FEED  = 0x55;

    //USB Clock divider
    USBCLKCFG = 5;                 // 5 == divide by 6
    
    //Clock Output
    CLKOUTCFG = 0;                 //No clock output
}