#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#include "clktime.h"

extern clktime ClkRefTai    (void);
extern clktime ClkNowTai    (void);
extern void    ClkNowTmLocal(struct tm* ptm);
extern void    ClkNowTmUtc  (struct tm* ptm);

extern void    ClkInit      (void);
extern void    ClkMain      (void);
