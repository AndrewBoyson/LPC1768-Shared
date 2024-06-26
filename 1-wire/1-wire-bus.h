#include <stdbool.h>

extern volatile int OneWireBusValue;
extern bool OneWireBusBusy(void);
extern void OneWireBusInit(char* pin);
extern void OneWireBusReset(void);
extern void OneWireBusWriteBit(int bit);
extern void OneWireBusWriteBitWithPullUp(int bit, int pullupms);
extern void OneWireBusReadBit(void);

extern volatile int OneWireBusLowTweak;
extern volatile int OneWireBusFloatTweak;
extern volatile int OneWireBusReadTweak;
extern volatile int OneWireBusHighTweak;
extern volatile int OneWireBusReleaseTweak;
