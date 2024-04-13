#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>

static uint32_t currentPositionInMessage;
static uint32_t bufferPositionInMessage;
static int bufferLength;
static char* pBuffer;
static char* p;

void HttpAddStart(uint32_t position, int mss, char *pData)
{
    currentPositionInMessage = 0;
    bufferPositionInMessage = position;
    bufferLength = mss;
    pBuffer = pData;
    p       = pData;
}
int HttpAddLength()
{
    return p - pBuffer;
}
bool HttpAddFilled()
{
    return p - pBuffer >= bufferLength;
}

void HttpAddChar(char c)
{
    //Add character if the current position is within the buffer
    if (currentPositionInMessage >= bufferPositionInMessage &&
        currentPositionInMessage <  bufferPositionInMessage + bufferLength) *p++ = c;
    
    currentPositionInMessage++;
}

void HttpAddFillChar (char c, int length)
{
    while (length > 0)
    {
        HttpAddChar(c);
        length--;
    }
}
int  HttpAddText  (const char* text)
{
	if (!text) return 0;
    const char* start = text;
    while (*text)
    {
        HttpAddChar(*text);
        text++;
    }
    return text - start;
}
int  HttpAddTextN (const char* text, int bufferLength)
{
	if (!text) return 0;
    const char* start = text;
    while (*text && text - start < bufferLength)
    {
        HttpAddChar(*text);
        text++;
    }
    return text - start;
}
int  HttpAddV     (char *fmt, va_list argptr)
{
    int size  = vsnprintf(NULL, 0, fmt, argptr); //Find the size required
    char text[size + 1];                         //Allocate enough memory for the size required with an extra byte for the terminating null
    vsprintf(text, fmt, argptr);                 //Fill the new buffer
    return HttpAddText(text);                    //Add the text
}
int  HttpAddF     (char *fmt, ...)
{
    va_list argptr;
    va_start(argptr, fmt);
    int size = HttpAddV(fmt, argptr);
    va_end(argptr);
    return size;
}
void HttpAddData  (const char* data, int length)
{
    while (length > 0)
    {
        HttpAddChar(*data);
        data++;
        length--;
    }
}
void HttpAddStream(void (*startFunction)(void), int (*enumerateFunction)(void))
{
    startFunction();
    while (true)
    {
        int c = enumerateFunction();
        if (c == EOF) break;
        HttpAddChar(c);
    }
}
void HttpAddNibbleAsHex(int nibble)
{
    nibble &= 0x0F;
    char c;
    if      (nibble < 0x0A) c = nibble + '0';
    else if (nibble < 0x10) c = nibble - 0xA + 'A';
    else                    c = '0';
    HttpAddChar(c);
}
void HttpAddByteAsHex(int value)
{
    HttpAddNibbleAsHex(value >> 4);
    HttpAddNibbleAsHex(value >> 0);
}
void HttpAddInt12AsHex(int value)
{   
    HttpAddNibbleAsHex(value >> 8);
    HttpAddNibbleAsHex(value >> 4);
    HttpAddNibbleAsHex(value >> 0);
}
void HttpAddInt16AsHex(int value)
{   
    HttpAddNibbleAsHex(value >> 12);
    HttpAddNibbleAsHex(value >>  8);
    HttpAddNibbleAsHex(value >>  4);
    HttpAddNibbleAsHex(value >>  0);
}
void HttpAddInt32AsHex(int value)
{   
    HttpAddNibbleAsHex(value >> 28);
    HttpAddNibbleAsHex(value >> 24);
    HttpAddNibbleAsHex(value >> 20);
    HttpAddNibbleAsHex(value >> 16);
    HttpAddNibbleAsHex(value >> 12);
    HttpAddNibbleAsHex(value >>  8);
    HttpAddNibbleAsHex(value >>  4);
    HttpAddNibbleAsHex(value >>  0);
}
void HttpAddInt64AsHex(int64_t value)
{   
    HttpAddNibbleAsHex(value >> 60);
    HttpAddNibbleAsHex(value >> 56);
    HttpAddNibbleAsHex(value >> 52);
    HttpAddNibbleAsHex(value >> 48);
    HttpAddNibbleAsHex(value >> 44);
    HttpAddNibbleAsHex(value >> 40);
    HttpAddNibbleAsHex(value >> 36);
    HttpAddNibbleAsHex(value >> 32);
    HttpAddNibbleAsHex(value >> 28);
    HttpAddNibbleAsHex(value >> 24);
    HttpAddNibbleAsHex(value >> 20);
    HttpAddNibbleAsHex(value >> 16);
    HttpAddNibbleAsHex(value >> 12);
    HttpAddNibbleAsHex(value >>  8);
    HttpAddNibbleAsHex(value >>  4);
    HttpAddNibbleAsHex(value >>  0);
}
void HttpAddBytesAsHex(const uint8_t* value, int size)
{
    int i = 0;
    while(true)
    {
        HttpAddByteAsHex(value[i]);
        i++;
        if (i >= size) break;
        if (i % 16 == 0) HttpAddText("\r\n");
        else             HttpAddChar(' ');
    }
}
void HttpAddBytesAsHexRev(const uint8_t* value, int size)
{
    int i = 0;
    while(true)
    {
        HttpAddByteAsHex(value[size - i - 1]);
        i++;
        if (i >= size) break;
        if (i % 16 == 0) HttpAddText("\r\n");
        else             HttpAddChar(' ');
    }
}
void HttpAddTm(struct tm* ptm)
{
    HttpAddF("%d-%02d-%02d ", ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday);
    switch(ptm->tm_wday)
    {
        case  0: HttpAddText("Sun"); break;
        case  1: HttpAddText("Mon"); break;
        case  2: HttpAddText("Tue"); break;
        case  3: HttpAddText("Wed"); break;
        case  4: HttpAddText("Thu"); break;
        case  5: HttpAddText("Fri"); break;
        case  6: HttpAddText("Sat"); break;
        default: HttpAddText("???"); break;
    }
    HttpAddF(" %02d:%02d:%02d", ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
    if      (ptm->tm_isdst  > 0) HttpAddText(" BST");
    else if (ptm->tm_isdst == 0) HttpAddText(" GMT");
    else                         HttpAddText(" UTC");
}
