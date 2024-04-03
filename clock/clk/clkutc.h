#include "clktime.h"

extern int     ClkUtcGetEpochOffset(void);
extern void    ClkUtcSetEpochOffsetWithUtcChange(int value);
extern void    ClkUtcAddEpochOffsetWithUtcChange(int value);
extern void    ClkUtcSetEpochOffsetWithoutUtcChange(int value);

extern int     ClkUtcGetNextEpochMonth1970(void); //Months since 1970 
extern clktime ClkUtcGetNextEpoch         (void);
extern void    ClkUtcSetNextEpochMonth1970(int value);

extern bool    ClkUtcGetNextLeapEnable (void);
extern bool    ClkUtcGetNextLeapForward(void);
extern void    ClkUtcSetNextLeapEnable (bool value);
extern void    ClkUtcSetNextLeapForward(bool value);
extern void    ClkUtcTglNextLeapEnable (void);
extern void    ClkUtcTglNextLeapForward(void);

extern void    ClkUtcInit(void);

extern clktime ClkUtcFromTai(clktime tai);
extern clktime ClkUtcToTai  (clktime utc);
extern void    ClkUtcCheckAdjustLeapSecondCount(clktime tai);