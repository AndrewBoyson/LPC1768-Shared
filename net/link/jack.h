#include <stdbool.h>

extern void JackLeds(bool phyLink, bool phySpeed, bool activity);
extern void JackInit(void (* linkLed)(char on), void (*speedLed)(char on));