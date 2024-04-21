#include "system.h"
#include "led.h"
#include "reset/reset.h"
#include "firmware.h"
#include "rtc/rtc.h"
#include "random/random.h"
#include "hrtimer/hrtimer.h"
#include "mstimer/mstimer.h"
#include "scan/scan.h"
#include "msrit.h"

void Lpc1768Init()
{
      SystemInit();
         LedInit();
       ResetInit();
       MsRitInit();
         RtcInit();
     HrTimerInit();
}
void Lpc1768Main()
{
       ResetMain();
    FirmwareMain();
      RandomMain();
     MsTimerMain();
        ScanMain();
}