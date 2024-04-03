
#define ALIAS4 0x42000000
#define BASE4  0x40000000
#define BIT_BAND4(ADDR_PAR, BIT_PAR) *((volatile unsigned *)(ALIAS4 + ((((ADDR_PAR - BASE4) << 3) + BIT_PAR) << 2)))

