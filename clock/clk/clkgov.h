#include <stdint.h>
#include <stdbool.h>

#include "../../lpc1768/tm/time64.h"
#include "clktime.h"

extern bool ClkGovTrace;

extern int32_t ClkGovGetSlew(void); extern void    ClkGovSetSlew(int32_t value);
extern int32_t ClkGovGetPpb (void); extern void    ClkGovSetPpb (int32_t value);

extern void ClkGovInit      (void);

extern int ClkGovFreqDivisor      ;
extern int ClkGovFreqChangeMaxPpb ;
extern int ClkGovFreqSyncedLimPpb ;
extern int ClkGovFreqSyncedHysPpb ;
extern int ClkGovSlewDivisor      ;
extern int ClkGovSlewChangeMaxMs  ;
extern int ClkGovSlewSyncedLimNs  ;
extern int ClkGovSlewSyncedHysNs  ;
extern int ClkGovSlewOffsetMaxSecs;

extern bool ClkGovIsReceivingTime;
extern bool ClkGovTimeIsSynced;
extern bool ClkGovRateIsSynced;
extern int  ClkGovIsSynced(void);

extern void ClkGovSyncPpsI(void);
extern void ClkGovSyncPpsN(time64 t);
extern void ClkGovSyncPpsZ(void);

extern void ClkGovSyncTime(clktime time);