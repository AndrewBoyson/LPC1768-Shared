#include <stdint.h>
#include "clk/clktime.h"

extern uint64_t NtpTimeStampFromClkTime(clktime  tai);
extern clktime  NtpTimeStampToClkTime  (uint64_t timestamp);
