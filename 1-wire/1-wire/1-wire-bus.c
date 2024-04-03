#include <stdint.h>
#include <stdbool.h>

#include "log/log.h"
#include "lpc1768/gpio.h"
#include "lpc1768/bitband.h"
#include "1-wire-this/1-wire-bus-pin.h"

#define IR_ADDR 0x40008000

#define IR    (*((volatile unsigned *) IR_ADDR))
#define TCR   (*((volatile unsigned *) 0x40008004))
#define TC    (*((volatile unsigned *) 0x40008008))
#define PR    (*((volatile unsigned *) 0x4000800C))
#define MCR   (*((volatile unsigned *) 0x40008014))
#define MR0   (*((volatile unsigned *) 0x40008018))
#define MR1   (*((volatile unsigned *) 0x4000801C))
#define MR2   (*((volatile unsigned *) 0x40008020))
#define MR3   (*((volatile unsigned *) 0x40008024))
#define CTCR  (*((volatile unsigned *) 0x40008070))

#define ISER0 (*((volatile unsigned *) 0xE000E100))

#define IR_MR0 BIT_BAND4(IR_ADDR, 0)
#define IR_MR1 BIT_BAND4(IR_ADDR, 1)
#define IR_MR2 BIT_BAND4(IR_ADDR, 2)
#define IR_MR3 BIT_BAND4(IR_ADDR, 3)

volatile int OneWireBusValue;
bool OneWireBusBusy()
{
    return TCR;
}
static volatile uint32_t startFloat        = 0;
static volatile uint32_t startRead         = 0;
static volatile uint32_t startHigh         = 0;
static volatile uint32_t startRelease      = 0;
volatile int OneWireBusLowTweak     = 0;
volatile int OneWireBusFloatTweak   = 0;
volatile int OneWireBusReadTweak    = 0;
volatile int OneWireBusHighTweak    = 0;
volatile int OneWireBusReleaseTweak = 0;

void OneWireBusHandler(void) //Interrupt handler
{
    //Handle match channel 0
    if (IR_MR0)
    {
        IR = 1;      //Writing a logic one to the corresponding IR bit will reset the interrupt. Writing a zero has no effect. See 21.6.1.
        BUS_DIR = 0; //Set the direction to 0 = input
        uint32_t now = TC;
        if (now > startFloat) OneWireBusFloatTweak--;
        if (now < startFloat) OneWireBusFloatTweak++;
    }
    //Handle match channel 1
    if (IR_MR1)
    {
        IR = 2;
        OneWireBusValue = BUS_PIN; //Read the bus value
        uint32_t now = TC;
        if (now > startRead) OneWireBusReadTweak--;
        if (now < startRead) OneWireBusReadTweak++;
    }
    //Handle match channel 2
    if (IR_MR2)
    {
        IR = 4;
        BUS_SET;     //Set the output to high
        BUS_DIR = 1; //Set the direction to 1 = output
        uint32_t now = TC;
        if (now > startHigh) OneWireBusHighTweak--;
        if (now < startHigh) OneWireBusHighTweak++;
    }
    //Handle match channel 3
    if (IR_MR3)
    {
        IR = 8;
        BUS_DIR = 0; //Set the direction to 0 = input
        uint32_t now = TC;
        if (now > startRelease) OneWireBusReleaseTweak--;
        if (now < startRelease) OneWireBusReleaseTweak++;
        TCR = 0; //Stop the timer and hence signal not busy
    }
}


static void start(uint32_t beforeFloat, uint32_t beforeRead, uint32_t beforeHigh, uint32_t beforeRelease)
{    
    //Drop out if there is no end specified
    if (!beforeRelease) return;
    
    //Prevent spurious interrupts and reset the clock to prevent a match 
    MCR  = 0; // 21.6.8 Match Control Register - Do nothing
    TCR  = 2; // 21.6.2 Timer Control Register - Reset TC and PC
    
    //Set up the match registers - a before interval of zero means don't do that
    uint32_t us = OneWireBusLowTweak;
    
    if (beforeFloat) { us += beforeFloat  ; startFloat   = us; MR0 = us + OneWireBusFloatTweak  ; MCR |= 00001; }
    if (beforeRead ) { us += beforeRead   ; startRead    = us; MR1 = us + OneWireBusReadTweak   ; MCR |= 00010; }
    if (beforeHigh ) { us += beforeHigh   ; startHigh    = us; MR2 = us + OneWireBusHighTweak   ; MCR |= 00100; }
                     { us += beforeRelease; startRelease = us; MR3 = us + OneWireBusReleaseTweak; MCR |= 01000; }

    //Start the timer
    TCR  = 1;     // 21.6.2 Timer Control Register - Enable TC and PC
       
    //Drive the bus low
    BUS_CLR;     //Set the output to low
    BUS_DIR = 1; //Set the direction to 1 = output
    uint32_t now = TC;
    if (now > OneWireBusLowTweak) OneWireBusLowTweak++;
    if (now < OneWireBusLowTweak) OneWireBusLowTweak--;
}

void OneWireBusInit()
{
    TCR     =    2; // 21.6.2 Timer Control Register - Reset TC and PC
    CTCR    =    0; // 21.6.3 Count Control Register - Timer mode
    PR      =    0; // 21.6.5 Prescale register      - No prescale
    MCR     =    0; // 21.6.8 Match Control Register - Do nothing
    TCR     =    0; // 21.6.2 Timer Control Register - Disable TC and PC
    
    ISER0  |= 0x04; //  6.5.1 bit2 == Interrupt set enable for timer 1.
}

//Delays
#define    RESET_BUS_LOW_US  480 * 96
#define    READ_PRESENCE_US   70 * 96
#define    RESET_RELEASE_US  410 * 96

#define  WRITE_0_BUS_LOW_US   60 * 96
#define  WRITE_0_RELEASE_US   10 * 96

#define  WRITE_1_BUS_LOW_US    6 * 96
#define  WRITE_1_RELEASE_US   64 * 96

#define READ_BIT_BUS_LOW_US    6 * 96
#define         READ_BIT_US    9 * 96
#define READ_BIT_RELEASE_US   55 * 96

void OneWireBusReset()
{
    start(RESET_BUS_LOW_US, READ_PRESENCE_US, 0, RESET_RELEASE_US);
}
void OneWireBusWriteBitWithPullUp(int bit, int pullupms)
{
    if (pullupms)
    {
        if (bit) start(0, 0, WRITE_1_BUS_LOW_US, pullupms * 96000);
        else     start(0, 0, WRITE_0_BUS_LOW_US, pullupms * 96000);
    }
    else
    {
        if (bit) start(WRITE_1_BUS_LOW_US, 0, 0, WRITE_1_RELEASE_US);
        else     start(WRITE_0_BUS_LOW_US, 0, 0, WRITE_0_RELEASE_US);
    }
}
void OneWireBusWriteBit(int bit)
{
    OneWireBusWriteBitWithPullUp(bit, 0);
}
void OneWireBusReadBit()
{
    start(READ_BIT_BUS_LOW_US, READ_BIT_US, 0, READ_BIT_RELEASE_US);
}

