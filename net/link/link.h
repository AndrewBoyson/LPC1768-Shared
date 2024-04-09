#include <stdbool.h>

extern bool LinkTrace;

extern void LinkMain(void);
extern void LinkInit(void (* linkLed)(char on), void (*speedLed)(char on));

