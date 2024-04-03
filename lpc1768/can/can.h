#include <stdint.h>
extern void (*CanReceive)(uint16_t id, int length, uint32_t dataA, uint32_t dataB);
extern void CanSend      (uint16_t id, int length, uint32_t dataA, uint32_t dataB);
extern void CanInit(void);
extern void CanMain(void);