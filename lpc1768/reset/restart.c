#include <stdio.h>
#include <string.h>
#include "restart.h"
#include "lpc1768-this/restart-this.h"
#include "lpc1768/semihost.h"

#define PERSIST (*((volatile unsigned *) 0x20083FFC)) //Make sure the startup scatter leaves a 4 byte space at this address

#define MAGIC 51 //0101 0001

//Continually set during the program then saved by the relevant fault handler if that fault occurs.
volatile int RestartZone  = 0; //0 to FF
volatile int RestartPoint = 0; //0 to FFFF

void Restart(int type)
{
    PERSIST = MAGIC << 24 | (type & 0xF) << 20 | (RestartZone & 0xFF) << 12 | (RestartPoint & 0xFFF);
    SemihostReset();
}

//Recorded during initialisation
int lastCause  = 0;
int lastZone   = 0;
int lastPoint  = 0;

//Called by the user interface
int  RestartGetLastCause () { return lastCause; } //Cause can be 0 to 15
int  RestartGetLastZone  () { return lastZone;  } //Zone  can be 0 to 255
int  RestartGetLastPoint () { return lastPoint; } //Point can be 0 to 4095

void RestartInit()
{    
    int magic = PERSIST >> 24 & 0x00FF;  //Magic number if valid data
    if (magic == MAGIC) //Data is valid
    {
        lastCause = PERSIST >> 20 & 0x000F; //Type  can be 0 to 15
        lastZone  = PERSIST >> 12 & 0x00FF; //Zone  can be 0 to 255
        lastPoint = PERSIST       & 0x0FFF; //Point can be 0 to 4095
    }
    else //Data is invalid
    {
        lastCause = RESTART_CAUSE_INVALID_DATA;
        lastZone  =  0;
        lastPoint =  0;
    }
    
    //If no routine overwrites the cause then we know it must be the push button.
    PERSIST = MAGIC << 24 | RESTART_CAUSE_RESET_BUTTON << 20;
}
const char* RestartGetCauseString()
{
    switch (lastCause)
    {
        case RESTART_CAUSE_RESET_BUTTON:     return "Reset button"   ;
        case RESTART_CAUSE_SOFTWARE_RESET:   return "Software reset" ;
        case RESTART_CAUSE_HARD_FAULT:       return "Hard fault"     ;
        case RESTART_CAUSE_DEFAULT_HANDLER:  return "Default handler";
        case RESTART_CAUSE_INVALID_DATA:     return "Invalid"        ;
        default:                             return "Unknown"        ;
    }
}
const char* RestartGetZoneString()
{
    switch (lastZone)
    {
        case RESTART_ZONE_NONE:  return "None" ;
        case RESTART_ZONE_INIT:  return "Init" ;
        case RESTART_ZONE_NET:   return "Net"  ;
        case RESTART_ZONE_CLOCK: return "Clock";
        default:                 return RestartThisGetZoneString();
    }
}
