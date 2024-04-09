#include <stdio.h>
#include <string.h>

#include "restart.h"
#include "lpc1768/semihost.h"
#include "rsid.h"

__attribute__((section(".persist"))) static unsigned _cause;   //Restart cause
__attribute__((section(".persist"))) static unsigned _pc;      //Fault location (program counter)restart


#define AIRCR      (*((volatile unsigned *) 0xE000ED0C)) //Application Interrupt and Reset Control Register

void Restart(unsigned cause, unsigned pc)
{
	_cause = cause;
	_pc    = pc;
	if (cause == RESTART_CAUSE_RELOAD_PROGRAM) SemihostReset();    //Reset everything to allow the boot loader to run
	else                                       AIRCR = 0x05FA0004; //System reset request - just processor
	while(1) {;} //Wait
}

//Recorded during initialisation
unsigned lastCause = 0;
unsigned lastPC    = 0;

//Called by the user interface
unsigned RestartGetLastCause () { return lastCause; }
unsigned RestartGetLastPC    () { return lastPC;    }

void RestartInit()
{	
	if (RsidPor) //Power has been off
	{
        lastCause = RESTART_CAUSE_POWER_UP;
        lastPC    = 0;
    }
	else
	{
		lastCause = _cause;
		lastPC    = _pc;
	}
	_cause    = RESTART_CAUSE_RESET_BUTTON;
	_pc       = 0;
}
const char* RestartGetCauseString()
{
    switch (lastCause)
    {
        case RESTART_CAUSE_RESET_BUTTON:     return "Reset button";
        case RESTART_CAUSE_RELOAD_PROGRAM:   return "Reload";
        case RESTART_CAUSE_HARD_FAULT:       return "Hard fault";
        case RESTART_CAUSE_DEFAULT_HANDLER:  return "Default handler";
        case RESTART_CAUSE_WATCHDOG_HANDLER: return "Watchdog";
        case RESTART_CAUSE_POWER_UP:         return "Power up";
        default:                             return "Unknown";
    }
}
