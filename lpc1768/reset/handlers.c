
#include "restart.h"
#include "lpc1768/debug.h"

void DefaultHandler()
{
    Restart(RESTART_CAUSE_DEFAULT_HANDLER);
}
void HardFaultHandler()
{
    Restart(RESTART_CAUSE_HARD_FAULT);
}