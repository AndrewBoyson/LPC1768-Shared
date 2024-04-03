#include <stdint.h>
#include <stdbool.h>

typedef int64_t clktime; //30 bits per second; 1 unit equates to about 1ns or about 1ppb

extern void     ClkTimeSaveSnapshot(void);
extern void     ClkTimesGetFromSnapshot(clktime* pInt, clktime* pAbs);

extern clktime  ClkTimeGet(void);
extern void     ClkTimeSet(clktime extClock);
extern bool     ClkTimeIsSet(void);

extern void     ClkTimeIncrementByOneSecond(uint32_t baseCount);

extern void     ClkTimeAdjustSeconds(int seconds);

#define CLK_TIME_ONE_SECOND_SHIFT 30
#define CLK_TIME_ONE_MS_ISH_SHIFT 20
#define CLK_TIME_ONE_SECOND (1UL << CLK_TIME_ONE_SECOND_SHIFT)
