#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <stdint.h>

#include "lpc1768/serialpc.h"
#include "lpc1768/reset/rsid.h"

#define BUFFER_LENGTH 0x4000
__attribute__((section(".usbram"))) static char buffer[BUFFER_LENGTH]; //Pop the buffer into the USB area


//#define BUFFER_LENGTH 0x100
//static char buffer[BUFFER_LENGTH];

__attribute__((section(".persist"))) static char* pPush; //Initialised in init
__attribute__((section(".persist"))) static char* pPull; //Initialised in init
__attribute__((section(".persist"))) static char* pUart; //Initialised in init

static bool enable = true;
static void (*tmFunction)(struct tm* ptm);
static bool useUart = false; //This is set during initialisation
       bool LogUart = true;  //Always start off with the uart running
       
static char* incrementPushPullPointer(char* p, char* buffer, int bufferLength)
{
    p++; //increment the pointer by one
    if (p == buffer + bufferLength) p = buffer; //if the pointer is now beyond the end then point it back to the start
    return p;
}
static void push(const char c)
{
    //Move the pull position if about to run into it
    char* pNext = incrementPushPullPointer(pPush, buffer, BUFFER_LENGTH);
    if (pNext == pPull) pPull = incrementPushPullPointer(pPull, buffer, BUFFER_LENGTH);
    
    //Add the character at the push position
    *pPush = c;
    pPush = incrementPushPullPointer(pPush, buffer, BUFFER_LENGTH);
}
void LogChar(const char c)
{
    //Only add if allowed
    if (!enable) return;
    
    //Work out if the character needs to be delimited
    bool delimited = false;
    if (c < ' ' && c != '\r' && c != '\n') delimited = true;
    if (c > 126) delimited = false;
    
    //Push the delimiter or the character
    if (delimited) push('^');
    else           push(c);
    
    //Stop if its not delimited
    if (!delimited) return;
    
    //Push the first digit
    char h = c >> 4;
    if (h < 10) h += '0';
    else        h += 'A' - 10;
    push(h);
        
    //Push the second digit
    h = c & 0x0F;
    if (h < 10) h += '0';
    else        h += 'A' - 10;
    push(h);
    
}
static char *pEnumerate;
void LogEnumerateStart()
{
    pEnumerate = pPull;
}
int LogEnumerate()
{
    if (pEnumerate == pPush) return -1;
    char c = *pEnumerate;
    pEnumerate = incrementPushPullPointer(pEnumerate, buffer, BUFFER_LENGTH); 
    return c;
}
void LogEnable(bool value)
{
    enable = value;
}
void LogClear()
{
	pPush = buffer;
	pPull = buffer;
	pUart = buffer;
}
void LogInit(void (*tmFunctionParam)(struct tm* ptm), int baud)
{
    useUart = baud;
    if (useUart) SerialPcInit(baud);
    tmFunction = tmFunctionParam;
	
	//If had power on reset then reset the persistent pointers to the start of the buffer
	if (RsidPor || 
	    pPush < buffer || pPush > buffer + BUFFER_LENGTH ||
	    pPull < buffer || pPull > buffer + BUFFER_LENGTH ||
	    pUart < buffer || pUart > buffer + BUFFER_LENGTH )
	{
		pPush = buffer;
		pPull = buffer;
		pUart = buffer;
	}
}
void LogMain()
{
    if (!useUart)       return;
    if (!LogUart)       return;     //Do nothing if uart is not enabled
    if (pUart == pPush) return;     //Do nothing if all characters have been sent
    int result = SerialPcPutC(*pUart); //Attempt to write the character
    if (result == 0) pUart = incrementPushPullPointer(pUart, buffer, BUFFER_LENGTH); //If the character was written to the uart then move to the next
}
int Log(const char* snd)
{
    const char* ptr = snd;
    while (*ptr) LogChar(*ptr++); //Send the string to the log buffer
    return ptr - snd;
}
int LogV(const char *fmt, va_list argptr)
{
    int size  = vsnprintf(NULL, 0, fmt, argptr);  //Find the size required
    char snd[size + 1];                           //Allocate enough memory for the size required with an extra byte for the terminating null
    vsprintf(snd, fmt, argptr);                   //Fill the new buffer
    return Log(snd);                              //Send the string to the log buffer
}
int LogF(const char *fmt, ...)
{
    va_list argptr;
    va_start(argptr, fmt);
    int size = LogV(fmt, argptr);
    va_end(argptr);
    return size;
}
static void pushuint4(int value)
{    
    if      (value > 9999) { LogChar('+'); LogChar('+'); LogChar('+'); LogChar('+'); }
    else if (value <    0) { LogChar('-'); LogChar('-'); LogChar('-'); LogChar('-'); }
    else
    {
        div_t divres;
        int k, c, t, u;
        divres = div(value      , 10); u = divres.rem;
        divres = div(divres.quot, 10); t = divres.rem;
        divres = div(divres.quot, 10); c = divres.rem;
                                       k = divres.quot;                           
        LogChar(k + '0'); LogChar(c + '0'); LogChar(t + '0'); LogChar(u + '0');
    }
}
static void pushuint3(int value)
{
    if      (value > 999) { LogChar('+'); LogChar('+'); LogChar('+'); }
    else if (value <   0) { LogChar('-'); LogChar('-'); LogChar('-'); }
    else
    {
        div_t divres;
        int c, t, u;
        divres = div(value      , 10); u = divres.rem;
        divres = div(divres.quot, 10); t = divres.rem;
                                       c = divres.quot;
        LogChar(c + '0'); LogChar(t + '0'); LogChar(u + '0');
    }
}
static void pushuint2(int value)
{
    if      (value > 99) { LogChar('+'); LogChar('+'); }
    else if (value <  0) { LogChar('-'); LogChar('-'); }
    else
    {
        div_t divres;
        int t, u;
        divres = div(value      , 10); u = divres.rem;
                                       t = divres.quot;
        LogChar(t + '0'); LogChar(u + '0');
    }
}

static int logTimeOnly()
{
    if (!tmFunction) return 0;
    
    struct tm tm;
    tmFunction(&tm);
    
    pushuint4(tm.tm_year + 1900);
    LogChar('-');
    pushuint3(tm.tm_yday + 1);
    LogChar(' ');
    pushuint2(tm.tm_hour);
    LogChar(':');
    pushuint2(tm.tm_min);
    LogChar(':');
    pushuint2(tm.tm_sec);
    return 17;
}
int LogTime(const char *snd)
{
    int size = 0;
    size += logTimeOnly();
    size++; LogChar(' ');
    size += Log(snd);
    return size;
}
int LogTimeF(const char *fmt, ...)
{
    int size = 0;
    va_list argptr;
    va_start(argptr, fmt);
    size = logTimeOnly();
    size++; LogChar(' ');
    size += LogV(fmt, argptr);
    va_end(argptr);
    return size;
}

void LogNibbleAsHex(int nibble)
{
    nibble &= 0x0F;
    char c;
    if      (nibble < 0x0A) c = nibble + '0';
    else if (nibble < 0x10) c = nibble - 0xA + 'A';
    else                    c = '0';
    LogChar(c);
}

void LogByteAsHex(int value)
{
    LogNibbleAsHex(value >> 4);
    LogNibbleAsHex(value >> 0);
}
void LogBytesAsHex(const uint8_t* bytes, int size)
{
    int i = 0;
    while(true)
    {
        LogByteAsHex(bytes[i]);
        i++;
        if (i >= size) break;
        if (i % 16 == 0) Log("\r\n");
        else             LogChar(' ');
    }
}
