#include <stdbool.h>

#define RSID      (*((volatile unsigned *) 0x400FC180))

bool RsidPor      = false;
bool RsidExtr     = false;
bool RsidWdtr     = false;
bool RsidBodr     = false;
bool RsidSysReset = false;
bool RsidLockup   = false;

void RsidInit()
{
    RsidPor      = RSID &  1; //RSID is defined in 3.4.1 Reset Source Identification Register
    RsidExtr     = RSID &  2;
    RsidWdtr     = RSID &  4;
    RsidBodr     = RSID &  8;
    RsidSysReset = RSID & 16;
    RsidLockup   = RSID & 32;
    RSID     = 0x3F;      //Clear any values
}