#include <stdbool.h>


extern void JackInit(char* linkLedPin, char* speedLedPin);
extern void JackLeds(bool phyLink, bool phySpeed, bool activity);