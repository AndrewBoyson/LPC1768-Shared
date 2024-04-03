#include <stdbool.h>
#include <stdarg.h>
#include <stdint.h>
#include <time.h>

#define HTTP_WAIT                    0
#define HTTP_FINISHED                1
#define HTTP_HAVE_SOMETHING_TO_SEND  2

extern void   HttpReset   (int connectionId);
extern bool   HttpAdd     (int connectionId,                      int* pWindowSize, char* pWindow, uint32_t windowPositionInStream); //returns true if finished; false if not
extern int    HttpPoll    (int connectionId, bool clientFinished);  //returns true if something to send; false if not
extern void   HttpRequest (int connectionId,                      int   windowSize, char* pWindow, uint32_t windowPositionInStream);

extern bool   HttpGetTrace(void);

extern void   HttpAddStart        (uint32_t position, int mss, char *pData);
extern int    HttpAddLength       (void);
extern bool   HttpAddFilled       (void);

extern void   HttpAddChar         (char c);
extern void   HttpAddFillChar     (char c, int length);
extern int    HttpAddText         (const char* text);
extern int    HttpAddTextN        (const char* text, int length);
extern int    HttpAddV            (char *fmt, va_list argptr);
extern int    HttpAddF            (char *fmt, ...);
extern void   HttpAddData         (const char* data, int length);
extern void   HttpAddStream       (void (*startFunction)(void), int (*enumerateFunction)(void));
extern void   HttpAddNibbleAsHex  (int value);
extern void   HttpAddByteAsHex    (int value);
extern void   HttpAddInt12AsHex   (int value);
extern void   HttpAddInt16AsHex   (int value);
extern void   HttpAddInt32AsHex   (int value);
extern void   HttpAddInt64AsHex   (int64_t value);
extern void   HttpAddBytesAsHex   (const uint8_t* value, int size);
extern void   HttpAddBytesAsHexRev(const uint8_t* value, int size);
extern void   HttpAddTm           (struct tm* ptm);

extern void   HttpOk(const char* contentType, const char* cacheControl, const char* lastModifiedDate, const char* lastModifiedTime);
extern char*  HttpOkCookieName;
extern char*  HttpOkCookieValue;
extern int    HttpOkCookieMaxAge;

extern void   HttpNotFound        (void);
extern void   HttpNotModified     (void);

extern int    HttpRequestRead(char *p, int len, char** ppMethod, char** ppPath, char** ppQuery, char** ppLastModified, char** ppCookies, int* pContentLength);

extern char*  HttpCookiesSplit      (char* pCookies, char** ppName, char** ppValue);
extern char*  HttpQuerySplit        (char* pQuery,   char** ppName, char** ppValue);
extern int    HttpQueryValueAsInt   (char* pValue);
extern double HttpQueryValueAsDouble(char* pValue);
extern void   HttpQueryUnencode     (char* pValue);

extern void   HttpDateFromDateTime(const char* date, const char *ptime, char* ptext);
extern void   HttpDateFromNow(char* pText);

extern bool HttpSameStr               (const char* pa,   const char* pb);
extern bool HttpSameStrCaseInsensitive(const char* pa,   const char* pb);
extern bool HttpSameDate              (const char* date, const char* time, const char* pOtherDate);

#define HTTP_DATE_LENGTH 30

