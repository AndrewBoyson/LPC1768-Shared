#include "register.h"

void GpioInit()
{
    PCONP |= 1 << 15; //GPIO
}