#include <stdint.h>
#include <stdbool.h>

extern bool I2C1Busy;

extern void I2C1Send(uint8_t addr, int length, uint8_t* pData, int* pResult);

extern void I2C1Init(void);
extern void I2C1Main(void);

#define I2C1_RESULT_NONE      0
#define I2C1_RESULT_OK        1
#define I2C1_RESULT_FAILED    2
#define I2C1_RESULT_TIMED_OUT 3