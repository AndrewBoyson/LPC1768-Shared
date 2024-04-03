#include <stdarg.h>
#include <time.h>
#include <stdbool.h>
#include <stdint.h>

extern bool LogUart;
extern void LogInit(void (*tmFunction)(struct tm* ptm), int baud); //Set baud to 0 if no serial
extern void LogMain(void);

extern void LogClear(void);

extern void LogChar(const char c);
extern int  Log(const char* snd);
extern int  LogV(const char *fmt, va_list argptr);
extern int  LogF(const char *fmt, ...);
extern int  LogTime(const char *snd);
extern int  LogTimeF(const char *fmt, ...);

extern void LogEnumerateStart(void);
extern int  LogEnumerate(void);
extern void LogEnable(bool value);

extern void LogNibbleAsHex(int nibble);
extern void LogByteAsHex(int value);
extern void LogBytesAsHex(const uint8_t* bytes, int size);
