#include <stdint.h>
#include <stdbool.h>

extern void Restart(unsigned cause, unsigned pc);

extern unsigned RestartGetLastCause(void);
extern unsigned RestartGetLastPC   (void);

extern void RestartInit    (void);

extern const char* RestartGetCauseString(void);

#define RESTART_CAUSE_RESET_BUTTON     0
#define RESTART_CAUSE_SEMIHOST_RESET   1
#define RESTART_CAUSE_HARD_FAULT       2
#define RESTART_CAUSE_DEFAULT_HANDLER  3
#define RESTART_CAUSE_WATCHDOG_HANDLER 4
#define RESTART_CAUSE_POWER_UP         5

