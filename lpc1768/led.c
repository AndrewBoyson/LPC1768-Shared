//#include <stdbool.h>

#include "gpio.h"

#define LED1BIT 18
#define LED2BIT 20
#define LED3BIT 21
#define LED4BIT 23

#define LED1DIR FIO1DIR(LED1BIT)
#define LED2DIR FIO1DIR(LED2BIT)
#define LED3DIR FIO1DIR(LED3BIT)
#define LED4DIR FIO1DIR(LED4BIT)

#define LED1PIN FIO1PIN(LED1BIT)
#define LED2PIN FIO1PIN(LED2BIT)
#define LED3PIN FIO1PIN(LED3BIT)
#define LED4PIN FIO1PIN(LED4BIT)

#define LED1SET FIO1SET(LED1BIT)
#define LED1CLR FIO1CLR(LED1BIT)

#define LED2SET FIO1SET(LED2BIT)
#define LED2CLR FIO1CLR(LED2BIT)

#define LED3SET FIO1SET(LED3BIT)
#define LED3CLR FIO1CLR(LED3BIT)

#define LED4SET FIO1SET(LED4BIT)
#define LED4CLR FIO1CLR(LED4BIT)

void LedInit()
{
    LED1DIR = 1;
    LED2DIR = 1;
    LED3DIR = 1;
    LED4DIR = 1;
}
void Led1Set(char value) { if (value) LED1SET; else LED1CLR; }
void Led2Set(char value) { if (value) LED2SET; else LED2CLR; }
void Led3Set(char value) { if (value) LED3SET; else LED3CLR; }
void Led4Set(char value) { if (value) LED4SET; else LED4CLR; }

void Led1Tgl() { if (LED1PIN) LED1CLR; else LED1SET; }
void Led2Tgl() { if (LED2PIN) LED2CLR; else LED2SET; }
void Led3Tgl() { if (LED3PIN) LED3CLR; else LED3SET; }
void Led4Tgl() { if (LED4PIN) LED4CLR; else LED4SET; }

char Led1Get() { return LED1PIN; }
char Led2Get() { return LED2PIN; }
char Led3Get() { return LED3PIN; }
char Led4Get() { return LED4PIN; }
