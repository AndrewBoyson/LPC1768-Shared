#include "led.h"

void LedInit()
{
    LED1DIR = 1;
    LED2DIR = 1;
    LED3DIR = 1;
    LED4DIR = 1;
}
void Led1Set(char value) { LED1DIR = 1; if (value) LED1SET; else LED1CLR; }
void Led2Set(char value) { LED2DIR = 1; if (value) LED2SET; else LED2CLR; }
void Led3Set(char value) { LED3DIR = 1; if (value) LED3SET; else LED3CLR; }
void Led4Set(char value) { LED4DIR = 1; if (value) LED4SET; else LED4CLR; }

void Led1Tgl() { if (LED1PIN) LED1CLR; else LED1SET; }
void Led2Tgl() { if (LED2PIN) LED2CLR; else LED2SET; }
void Led3Tgl() { if (LED3PIN) LED3CLR; else LED3SET; }
void Led4Tgl() { if (LED4PIN) LED4CLR; else LED4SET; }

char Led1Get() { return LED1PIN; }
char Led2Get() { return LED2PIN; }
char Led3Get() { return LED3PIN; }
char Led4Get() { return LED4PIN; }
