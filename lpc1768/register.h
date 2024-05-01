#include "bitband.h"

#define PCONP     (*((volatile unsigned *) 0x400FC0C4))
#define PCLKSEL0  (*((volatile unsigned *) 0x400FC1A8))
#define PCLKSEL1  (*((volatile unsigned *) 0x400FC1AC))
#define PINSEL0   (*((volatile unsigned *) 0x4002C000))
#define PINSEL1   (*((volatile unsigned *) 0x4002C004))
#define PINSEL2   (*((volatile unsigned *) 0x4002C008))
#define PINSEL3   (*((volatile unsigned *) 0x4002C00C))
#define PINSEL4   (*((volatile unsigned *) 0x4002C010))

//OSC
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

#define   FLASHCFG *((volatile unsigned *)  FLASHCFG_ADDR)
#define    CCLKCFG *((volatile unsigned *)   CCLKCFG_ADDR)
#define  CLKSRCSEL *((volatile unsigned *) CLKSRCSEL_ADDR)
#define    PLL0CFG *((volatile unsigned *)   PLL0CFG_ADDR)
#define   PLL0FEED *((volatile unsigned *)  PLL0FEED_ADDR)
#define  USBCLKCFG *((volatile unsigned *) USBCLKCFG_ADDR)
#define  CLKOUTCFG *((volatile unsigned *) CLKOUTCFG_ADDR)

#define PLLE0    BIT_BAND4( PLL0CON_ADDR,  0)
#define PLLC0    BIT_BAND4( PLL0CON_ADDR,  1)
#define PLOCK0   BIT_BAND4(PLL0STAT_ADDR, 26) //Reflects the PLL0 Lock status.
#define OSCRANGE BIT_BAND4(     SCS_ADDR,  4)
#define OSCEN    BIT_BAND4(     SCS_ADDR,  5)
#define OSCSTAT  BIT_BAND4(     SCS_ADDR,  6)

//UART0
#define U0RBR_ADDR 0x4000C000 //Receiver Buffer Register   (DLAB = 0 RO common RBR THR DLL)
#define U0THR_ADDR 0x4000C000 //Transmit Holding Register  (DLAB = 0 WO common RBR THR DLL)
#define U0DLL_ADDR 0x4000C000 //Divisor Latch LSB register (DLAB = 1 RW common RBR THR DLL)
#define U0DLM_ADDR 0x4000C004 //Divisor Latch MSB register (DLAB = 1 RW common IER DLM)
#define U0IER_ADDR 0x4000C004 //Divisor Latch MSB register (DLAB = 0 RW common IER DLM)
#define U0FCR_ADDR 0x4000C008 //FIFO Control Register
#define U0LCR_ADDR 0x4000C00C //Line Control Register
#define U0LSR_ADDR 0x4000C014 //Line Status Register
#define U0FDR_ADDR 0x4000C028 //Fractional Divider Register

#define U0_RECEIVE_BUFFER_REGISTER     *((volatile unsigned *)   U0RBR_ADDR)
#define U0_TRANSMIT_HOLDING_REGISTER   *((volatile unsigned *)   U0THR_ADDR)
#define U0_LINE_CONTROL_REGISTER       *((volatile unsigned *)   U0LCR_ADDR)
#define U0_DIVISOR_LSB                 *((volatile unsigned *)   U0DLL_ADDR)
#define U0_DIVISOR_MSB                 *((volatile unsigned *)   U0DLM_ADDR)
#define U0_FRACTIONAL_DIVIDER_REGISTER *((volatile unsigned *)   U0FDR_ADDR)

#define U0_FIFO_ENABLE                     BIT_BAND4(U0FCR_ADDR,  0)
#define U0_DIVISOR_ACCESS_BIT              BIT_BAND4(U0LCR_ADDR,  7) //Divisor Latch Access Bit
#define U0_RECEIVER_DATA_READY             BIT_BAND4(U0LSR_ADDR,  0) //Receiver Data Ready
#define U0_TRANSMIT_HOLDING_REGISTER_EMPTY BIT_BAND4(U0LSR_ADDR,  5) //Transmitter Holding Register Empty

//UART1
#define U1RBR_ADDR 0x40010000 //Receiver Buffer Register   (DLAB = 0 RO common RBR THR DLL)
#define U1THR_ADDR 0x40010000 //Transmit Holding Register  (DLAB = 0 WO common RBR THR DLL)
#define U1DLL_ADDR 0x40010000 //Divisor Latch LSB register (DLAB = 1 RW common RBR THR DLL)
#define U1DLM_ADDR 0x40010004 //Divisor Latch MSB register (DLAB = 1 RW common IER DLM)
#define U1IER_ADDR 0x40010004 //Interrupt Enable Register  (DLAB = 0 RW common IER DLM)
#define U1FCR_ADDR 0x40010008 //FIFO Control Register
#define U1LCR_ADDR 0x4001000C //Line Control Register
#define U1LSR_ADDR 0x40010014 //Line Status Register
#define U1FDR_ADDR 0x40010028 //Fractional Divider Register

#define U1_RECEIVE_BUFFER_REGISTER     *((volatile unsigned *)   U1RBR_ADDR)
#define U1_TRANSMIT_HOLDING_REGISTER   *((volatile unsigned *)   U1THR_ADDR)
#define U1_LINE_CONTROL_REGISTER       *((volatile unsigned *)   U1LCR_ADDR)
#define U1_DIVISOR_LSB                 *((volatile unsigned *)   U1DLL_ADDR)
#define U1_DIVISOR_MSB                 *((volatile unsigned *)   U1DLM_ADDR)
#define U1_FRACTIONAL_DIVIDER_REGISTER *((volatile unsigned *)   U1FDR_ADDR)

#define U1_FIFO_ENABLE                     BIT_BAND4(U1FCR_ADDR,  0)
#define U1_DIVISOR_ACCESS_BIT              BIT_BAND4(U1LCR_ADDR,  7) //Divisor Latch Access Bit
#define U1_RECEIVER_DATA_READY             BIT_BAND4(U1LSR_ADDR,  0) //Receiver Data Ready
#define U1_TRANSMIT_HOLDING_REGISTER_EMPTY BIT_BAND4(U1LSR_ADDR,  5) //Transmitter Holding Register Empty

//RIT - heating push button debounce
#define RICOMPVAL   (*((volatile unsigned *) 0x400B0000)) //Compare register R/W 0xFFFF FFFF
#define RIMASK      (*((volatile unsigned *) 0x400B0004)) //Mask    register R/W 0
#define RICTRL      (*((volatile unsigned *) 0x400B0008)) //Control register R/W 0xC
#define RICOUNTER   (*((volatile unsigned *) 0x400B000C)) //32-bit  counter  R/W 0

//PWM1 - heating pump speed
#define PWM1TCR     (*((volatile unsigned *) 0x40018004))
#define PWM1TC      (*((volatile unsigned *) 0x40018008))
#define PWM1PR      (*((volatile unsigned *) 0x4001800C))
#define PWM1MCR     (*((volatile unsigned *) 0x40018014))
#define PWM1MR0     (*((volatile unsigned *) 0x40018018))
#define PWM1MR1     (*((volatile unsigned *) 0x4001801C))
#define PWM1PCR     (*((volatile unsigned *) 0x4001804C))
#define PWM1LER     (*((volatile unsigned *) 0x40018050))
#define PWM1CTCR    (*((volatile unsigned *) 0x40018070))

//TIM0 - hrtimer and hence mstimer and clock
#define T0TCR       (*((volatile unsigned *) 0x40004004))
#define T0TC        (*((volatile unsigned *) 0x40004008))
#define T0PR        (*((volatile unsigned *) 0x4000400C))
#define T0MCR       (*((volatile unsigned *) 0x40004014))
#define T0CTCR      (*((volatile unsigned *) 0x40004070))

//TIM1 - gps nmea interface
#define T1IR_ADDR 0x40008000

#define T1IR        (*((volatile unsigned *) T1IR_ADDR ))
#define T1TCR       (*((volatile unsigned *) 0x40008004))
#define T1TC        (*((volatile unsigned *) 0x40008008))
#define T1PR        (*((volatile unsigned *) 0x4000800C))
#define T1MCR       (*((volatile unsigned *) 0x40008014))
#define T1MR0       (*((volatile unsigned *) 0x40008018))
#define T1MR1       (*((volatile unsigned *) 0x4000801C))
#define T1MR2       (*((volatile unsigned *) 0x40008020))
#define T1MR3       (*((volatile unsigned *) 0x40008024))
#define T1CTCR      (*((volatile unsigned *) 0x40008070))

#define T1IR_MR0 BIT_BAND4(T1IR_ADDR, 0)
#define T1IR_MR1 BIT_BAND4(T1IR_ADDR, 1)
#define T1IR_MR2 BIT_BAND4(T1IR_ADDR, 2)
#define T1IR_MR3 BIT_BAND4(T1IR_ADDR, 3)

//WDT
#define WDMOD    *((volatile unsigned *) 0x40000000) //Mode
#define WDTC     *((volatile unsigned *) 0x40000004) //Timer constant
#define WDFEED   *((volatile unsigned *) 0x40000008) //Writing 0xAA followed by 0x55 here reloads the Watchdog timer with the value contained in WDTC.
#define WDCLKSEL *((volatile unsigned *) 0x40000010) //Clock source selection

//SSP1 - fram
#define SSP1CR0     (*((volatile unsigned *) 0x40030000))
#define SSP1CR1     (*((volatile unsigned *) 0x40030004))
#define SSP1DR      (*((volatile unsigned *) 0x40030008))
#define SSP1SR      (*((volatile unsigned *) 0x4003000C))
#define SSP1CPSR    (*((volatile unsigned *) 0x40030010))

// RTC
#define ILR         (*((volatile unsigned *) 0x40024000))
#define CCR         (*((volatile unsigned *) 0x40024008))
#define CIIR        (*((volatile unsigned *) 0x4002400C))
#define AMR         (*((volatile unsigned *) 0x40024010))
#define AUX         (*((volatile unsigned *) 0x4002405C))
#define AUXEN       (*((volatile unsigned *) 0x40024058))

#define SEC         (*((volatile unsigned *) 0x40024020))
#define MIN         (*((volatile unsigned *) 0x40024024))
#define HOUR        (*((volatile unsigned *) 0x40024028))
#define DOM         (*((volatile unsigned *) 0x4002402C))
#define DOW         (*((volatile unsigned *) 0x40024030))
#define DOY         (*((volatile unsigned *) 0x40024034))
#define MONTH       (*((volatile unsigned *) 0x40024038))
#define YEAR        (*((volatile unsigned *) 0x4002403C))
#define CALIBRATION (*((volatile unsigned *) 0x40024040))

//Interrupt Set-Enable
#define ISER0       (*((volatile unsigned *) 0xE000E100))
#define ICER0       (*((volatile unsigned *) 0xE000E180))

