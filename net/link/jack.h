#include <stdbool.h>

extern volatile unsigned* JackLinkLedDirPtr;
extern volatile unsigned* JackLinkLedSetPtr;
extern volatile unsigned* JackLinkLedClrPtr;
extern volatile unsigned* JackSpeedLedDirPtr;
extern volatile unsigned* JackSpeedLedSetPtr;
extern volatile unsigned* JackSpeedLedClrPtr;

extern void JackLeds(bool phyLink, bool phySpeed, bool activity);