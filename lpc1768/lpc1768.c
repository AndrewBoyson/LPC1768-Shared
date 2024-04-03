#include "system.h"
#include "led.h"
#include "reset/reset.h"
#include "firmware.h"
#include "lpc1768-this/lpc1768-this.h"
#include "rtc/rtc.h"
#include "random/random.h"
#include "hrtimer/hrtimer.h"
#include "mstimer/mstimer.h"
#include "scan/scan.h"
#include "bignum/bignum.h"

void Lpc1768Init()
{
         SystemInit();
            LedInit();
          ResetInit();
    Lpc1768ThisInit();
            RtcInit();
        HrTimerInit();
}
void Lpc1768Main()
{
          ResetMain();
       FirmwareMain();
    Lpc1768ThisMain();
         RandomMain();
        MsTimerMain();
           ScanMain();
}