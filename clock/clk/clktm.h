#include <time.h>
#include <stdint.h>

extern void    ClkTimeToTmLocal(clktime time, struct tm* ptm);
extern void    ClkTimeToTmUtc  (clktime time, struct tm* ptm);
extern clktime ClkTimeFromTmUtc(              struct tm* ptm);