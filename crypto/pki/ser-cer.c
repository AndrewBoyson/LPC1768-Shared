#include <stdint.h>

#include "base64.h"
#include "log/log.h"

uint8_t SerCerData[1024];
int     SerCerSize = 0;

static const char* pNext;
static const char* buffer = 
#include "web-this/ser-cer.inc"
;
static char readChar()
{
    char c = *pNext;
    if (!c) return 0;
    pNext++;
    return c;
}
static void readData()
{
    SerCerSize = 0;
    while (true)
    {
        int c = Base64ReadByte();
        if (c == -1) return;       //EOF or an error
        SerCerData[SerCerSize] = c;
        SerCerSize++;
    }
}
void SerCerInit()
{
    
    pNext = buffer;
    Base64ReadNextCharFunctionPointer = readChar;
    
    int r = Base64SkipLine();
    if (r < 0) return;
    
    readData();
/*
    LogF("Server certificate content - %d bytes\r\n", SerCerSize);
    LogBytesAsHex(SerCerData, SerCerSize); Log("\n\n");
*/
}