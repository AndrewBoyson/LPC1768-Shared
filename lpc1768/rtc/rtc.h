#include <time.h>
#include <stdbool.h>

extern bool RtcIsSet(void);
extern bool RtcPowerLost(void);

extern void RtcGetTm(struct tm* ptm);
extern void RtcSetTm(struct tm* ptm);

extern void RtcInit(void);