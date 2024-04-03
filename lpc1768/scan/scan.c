#include <stdint.h>
#include "lpc1768/hrtimer/hrtimer.h"

uint32_t ScanAverage = 0;
uint32_t ScanMinimum = 10000;
uint32_t ScanMaximum = 0;

void ScanMain()
{
    //Establish this scan time
    static uint32_t scanTimer = 0;

    bool firstScan = !scanTimer;

    uint32_t elapsed = HrTimerSinceRepetitive(&scanTimer);

    if (firstScan) return;

    //Average the scan time
    if (elapsed > ScanAverage) ScanAverage++;
    if (elapsed < ScanAverage) ScanAverage--;
    if (elapsed > ScanMaximum) ScanMaximum = elapsed;
    if (elapsed < ScanMaximum) ScanMaximum--;
    if (elapsed < ScanMinimum) ScanMinimum = elapsed;
}
