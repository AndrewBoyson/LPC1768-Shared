#include <stdbool.h>

extern bool FramEmpty;
extern int  FramUsed;
extern int  FramInit(void);
extern int  FramAllocate(int size);
extern void FramWrite(int address, int len, void* pVoid);
extern void FramRead (int address, int len, void* pVoid);
extern int  FramLoad (int len, void* pValue, void* pDefault);

#define FRAM_SIZE   8192
