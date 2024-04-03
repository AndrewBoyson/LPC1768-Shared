#include <stdint.h>

#include "lpc1768/hrtimer/hrtimer.h"
#include "random.h"

/*
Uses the low 8 bits of the hrtimer to give the number of cycles per scan.
The cycles per scan depends on many factors including network activity.
Network activity has a high entropy as it depends on many machines and human activity.
The minimum scan is typically at least 5000 so taking the low 8 bits should be indeterminate.
*/

#define RANDOM_LENGTH 64

uint8_t bytes[RANDOM_LENGTH];

uint8_t RandomGetByte()
{
    static uint8_t* p = bytes;
    uint8_t byte = *p;
    p++;
    if (p >= bytes + RANDOM_LENGTH) p = bytes;
    return byte;
}

void RandomMain()
{
    //Establish the scan time
    static uint32_t scanTimer = 0;
    uint32_t elapsed = HrTimerSinceRepetitive(&scanTimer);
    
    //Make the random number using the low 8 bits of the scan time as entropy
    static uint8_t* p = bytes;
    *p ^= elapsed & 0xFF;
    p++;
    if (p >= bytes + RANDOM_LENGTH) p = bytes;
}