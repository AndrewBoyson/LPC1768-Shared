#define FIO0DIR_ADDR 0x2009C000
#define FIO1DIR_ADDR 0x2009C020
#define FIO2DIR_ADDR 0x2009C040

#define FIO0PIN_ADDR 0x2009C014
#define FIO1PIN_ADDR 0x2009C034
#define FIO2PIN_ADDR 0x2009C054

#define FIO0SET_ADDR 0x2009C018
#define FIO1SET_ADDR 0x2009C038
#define FIO2SET_ADDR 0x2009C058

#define FIO0CLR_ADDR 0x2009C01C
#define FIO1CLR_ADDR 0x2009C03C
#define FIO2CLR_ADDR 0x2009C05C

#define FIO0SET(BIT_PAR) *((volatile unsigned *) FIO0SET_ADDR) = 1U << BIT_PAR
#define FIO1SET(BIT_PAR) *((volatile unsigned *) FIO1SET_ADDR) = 1U << BIT_PAR
#define FIO2SET(BIT_PAR) *((volatile unsigned *) FIO2SET_ADDR) = 1U << BIT_PAR

#define FIO0CLR(BIT_PAR) *((volatile unsigned *) FIO0CLR_ADDR) = 1U << BIT_PAR
#define FIO1CLR(BIT_PAR) *((volatile unsigned *) FIO1CLR_ADDR) = 1U << BIT_PAR
#define FIO2CLR(BIT_PAR) *((volatile unsigned *) FIO2CLR_ADDR) = 1U << BIT_PAR

#define ALIAS2 0x22000000
#define BASE2  0x20000000
#define BIT_BAND2(ADDR_PAR, BIT_PAR) *((volatile unsigned *)(ALIAS2 + ((((ADDR_PAR - BASE2) << 3) + BIT_PAR) << 2)))

#define FIO0PIN(BIT_PAR) BIT_BAND2(FIO0PIN_ADDR, BIT_PAR)
#define FIO1PIN(BIT_PAR) BIT_BAND2(FIO1PIN_ADDR, BIT_PAR)
#define FIO2PIN(BIT_PAR) BIT_BAND2(FIO2PIN_ADDR, BIT_PAR)

#define FIO0DIR(BIT_PAR) BIT_BAND2(FIO0DIR_ADDR, BIT_PAR)
#define FIO1DIR(BIT_PAR) BIT_BAND2(FIO1DIR_ADDR, BIT_PAR)
#define FIO2DIR(BIT_PAR) BIT_BAND2(FIO2DIR_ADDR, BIT_PAR)

extern void (*GpioHook)(void);
extern void GpioInit(void);
extern void GpioInterruptsEnable();
extern void GpioInterruptsDisable();