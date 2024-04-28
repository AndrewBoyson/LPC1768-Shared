#include <stdint.h>
#include <time.h>

#include "lpc1768/tm/tm.h"
#include "clktime.h"
#include "clkutc.h"

void    ClkTimeToTmLocal(clktime time, struct tm* ptm) {            TmLocalFromTime64(ClkUtcFromTai(time) >> CLK_TIME_ONE_SECOND_SHIFT, ptm); }
void    ClkTimeToTmUtc  (clktime time, struct tm* ptm) {              TmUtcFromTime64(ClkUtcFromTai(time) >> CLK_TIME_ONE_SECOND_SHIFT, ptm); }
clktime ClkTimeFromTmUtc(              struct tm* ptm) { return ClkUtcToTai(((clktime)TmUtcToTime64(ptm)) << CLK_TIME_ONE_SECOND_SHIFT)      ; }