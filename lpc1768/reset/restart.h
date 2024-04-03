#include <stdint.h>
#include <stdbool.h>

extern volatile int  RestartZone;
extern volatile int  RestartPoint;

extern void Restart(int type);

extern int  RestartGetLastZone (void);
extern int  RestartGetLastCause(void);
extern int  RestartGetLastPoint(void);

extern void RestartInit    (void);

extern const char* RestartGetCauseString(void);
#define RESTART_CAUSE_RESET_BUTTON     0
#define RESTART_CAUSE_SOFTWARE_RESET   1
#define RESTART_CAUSE_HARD_FAULT       2
#define RESTART_CAUSE_DEFAULT_HANDLER  3
#define RESTART_CAUSE_INVALID_DATA     4

extern const char* RestartGetZoneString(void);
#define RESTART_ZONE_NONE     0
#define RESTART_ZONE_INIT     1
#define RESTART_ZONE_NET      2
#define RESTART_ZONE_CLOCK    3
#define RESTART_ZONE_LOG      4
#define RESTART_ZONE_LPC1768  5
#define RESTART_ZONE_CRYPTO   6
#define RESTART_ZONE_DERIVED 10
