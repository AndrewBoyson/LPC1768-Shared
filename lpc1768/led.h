
#include "gpio.h"

#define LED1BIT 18
#define LED2BIT 20
#define LED3BIT 21
#define LED4BIT 23

#define LED1DIR FIO1DIR_ALIAS(LED1BIT)
#define LED2DIR FIO1DIR_ALIAS(LED2BIT)
#define LED3DIR FIO1DIR_ALIAS(LED3BIT)
#define LED4DIR FIO1DIR_ALIAS(LED4BIT)

#define LED1PIN FIO1PIN_ALIAS(LED1BIT)
#define LED2PIN FIO1PIN_ALIAS(LED2BIT)
#define LED3PIN FIO1PIN_ALIAS(LED3BIT)
#define LED4PIN FIO1PIN_ALIAS(LED4BIT)

#define LED1SET FIO1SET_BIT(LED1BIT)
#define LED1CLR FIO1CLR_BIT(LED1BIT)

#define LED2SET FIO1SET_BIT(LED2BIT)
#define LED2CLR FIO1CLR_BIT(LED2BIT)

#define LED3SET FIO1SET_BIT(LED3BIT)
#define LED3CLR FIO1CLR_BIT(LED3BIT)

#define LED4SET FIO1SET_BIT(LED4BIT)
#define LED4CLR FIO1CLR_BIT(LED4BIT)

extern void LedInit(void);

extern void Led1Set(char value);
extern void Led2Set(char value);
extern void Led3Set(char value);
extern void Led4Set(char value);

extern void Led1Tgl(void);
extern void Led2Tgl(void);
extern void Led3Tgl(void);
extern void Led4Tgl(void);

extern char Led1Get(void);
extern char Led2Get(void);
extern char Led3Get(void);
extern char Led4Get(void);