
#include "restart.h"
#include "lpc1768/debug.h"
#include "lpc1768/led.h"

static void debugFault(uint32_t *stackAddress)
{
volatile uint32_t r0;
volatile uint32_t r1;
volatile uint32_t r2;
volatile uint32_t r3;
volatile uint32_t r12;
volatile uint32_t lr; /* Link register. */
volatile uint32_t pc; /* Program counter. */
volatile uint32_t psr;/* Program status register. */

    r0  = stackAddress[0];
    r1  = stackAddress[1];
    r2  = stackAddress[2];
    r3  = stackAddress[3];
    r12 = stackAddress[4];
    lr  = stackAddress[5];
    pc  = stackAddress[6];
    psr = stackAddress[7];
	
	DebugWriteText("r0  = "); DebugWriteHex(r0 ); DebugWriteChar('\r');
	DebugWriteText("r1  = "); DebugWriteHex(r1 ); DebugWriteChar('\r');
	DebugWriteText("r2  = "); DebugWriteHex(r2 ); DebugWriteChar('\r');
	DebugWriteText("r3  = "); DebugWriteHex(r3 ); DebugWriteChar('\r');
	DebugWriteText("r12 = "); DebugWriteHex(r12); DebugWriteChar('\r');
	DebugWriteText("LR  = "); DebugWriteHex(lr ); DebugWriteChar('\r');
	DebugWriteText("PC  = "); DebugWriteHex(pc ); DebugWriteChar('\r');
	DebugWriteText("xPSR= "); DebugWriteHex(psr); DebugWriteChar('\r');
	DebugWriteChar('\r');
	DebugWriteChar('\r');
}
static uint32_t extractPC(uint32_t *stackAddress)
{
	return stackAddress[6];
}

void hardFaultHandler(uint32_t *stackAddress) {	LED3DIR = 1; LED3SET; debugFault(stackAddress); Restart(RESTART_CAUSE_HARD_FAULT      , extractPC(stackAddress)); }
void  watchdogHandler(uint32_t *stackAddress) {	LED2DIR = 1; LED2SET; debugFault(stackAddress); Restart(RESTART_CAUSE_WATCHDOG_HANDLER, extractPC(stackAddress)); }
void   defaultHandler(uint32_t *stackAddress) {	LED1DIR = 1; LED1SET; debugFault(stackAddress); Restart(RESTART_CAUSE_DEFAULT_HANDLER , extractPC(stackAddress)); }

 __attribute__((naked)) void HardFaultHandler(void)
{
    __asm volatile
    (
        " tst lr, #4                                         \n"
        " ite eq                                             \n"
        " mrseq r0, msp                                      \n"
        " mrsne r0, psp                                      \n"
        " ldr r1, [r0, #24]                                  \n"
        " ldr r2, hard_fault_handler_address                 \n"
        " bx r2                                              \n"
        " hard_fault_handler_address: .word hardFaultHandler \n"
    );	
}

 __attribute__((naked)) void WatchdogHandler()
{
    __asm volatile
    (
        " tst lr, #4                                     \n"
        " ite eq                                         \n"
        " mrseq r0, msp                                  \n"
        " mrsne r0, psp                                  \n"
        " ldr r1, [r0, #24]                              \n"
        " ldr r2, watchdog_handler_address               \n"
        " bx r2                                          \n"
        " watchdog_handler_address: .word watchdogHandler\n"
    );
}
 __attribute__((naked)) void DefaultHandler()
{
    __asm volatile
    (
        " tst lr, #4                                     \n"
        " ite eq                                         \n"
        " mrseq r0, msp                                  \n"
        " mrsne r0, psp                                  \n"
        " ldr r1, [r0, #24]                              \n"
        " ldr r2, default_handler_address                \n"
        " bx r2                                          \n"
        " default_handler_address: .word defaultHandler  \n"
    );
}
