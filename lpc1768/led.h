#include <stdbool.h>

extern void LedInit(void);

extern void Led1Set(bool value);
extern void Led2Set(bool value);
extern void Led3Set(bool value);
extern void Led4Set(bool value);

extern void Led1Tgl(void);
extern void Led2Tgl(void);
extern void Led3Tgl(void);
extern void Led4Tgl(void);

extern bool Led1Get(void);
extern bool Led2Get(void);
extern bool Led3Get(void);
extern bool Led4Get(void);