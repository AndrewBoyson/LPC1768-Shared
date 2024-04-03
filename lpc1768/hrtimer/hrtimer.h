#include <stdint.h>
#include <stdbool.h>

extern uint32_t HrTimerCount(void);

extern bool     HrTimerRepetitiveTick (uint32_t* pLastCount, uint32_t interval);

extern uint32_t HrTimerSinceRepetitive(uint32_t* pLastCount);
extern uint32_t HrTimerSince          (uint32_t   lastCount);
extern uint32_t HrTimerSinceMs        (uint32_t   lastCount);

extern uint32_t HrTimerProRata(uint32_t value, uint32_t part);
extern void     HrTimerInit(void);

#define HR_TIMER_COUNT_PER_SECOND 96000000UL
#define HR_TIMER_COUNT_PER_MS        96000UL
