#include <stdbool.h>
#include <stdint.h>

#include "lpc1768/random/random.h"

#define SESSION_ID_BIT_LENGTH 36

static char sessionId[(SESSION_ID_BIT_LENGTH + 5) / 6 + 1]; //Bit lengths not divisible by 6 require an extra space

void WebLoginSessionIdNew()
{
    char acc = 0;
    int srcByteBefore = -1;
    uint8_t randomByte = 0;
    
    for (int i = 0; i < SESSION_ID_BIT_LENGTH; i++)
    {
        int     srcByte = i / 8; 
        int     srcBit  = i - srcByte * 8;
        uint8_t srcMask = 1 << srcBit;
        
        int     dstByte = i / 6;
        int     dstBit  = i - dstByte * 6;
        uint8_t dstMask = 1 << dstBit;
        
        //Reset the accumulator to zero at start of a 6 bit word
        if (!dstBit) acc = 0;
        
        //Fetch a new random byte whenever the src byte changes
        if (srcByte != srcByteBefore) randomByte = RandomGetByte();
        srcByteBefore = srcByte;
        
        //Add the bit to the accumulator
        if (randomByte & srcMask) acc |= dstMask;
        
        //Convert the accumulator to base64 and store in the session Id
        if (dstBit == 5 || i == SESSION_ID_BIT_LENGTH - 1)
        {
            if      (acc < 26) sessionId[dstByte] = acc -  0 + 'A';
            else if (acc < 52) sessionId[dstByte] = acc - 26 + 'a';
            else if (acc < 62) sessionId[dstByte] = acc - 52 + '0';
            else if (acc < 63) sessionId[dstByte] =            '+';
            else               sessionId[dstByte] =            '/';
        }
    }
    sessionId[(SESSION_ID_BIT_LENGTH + 5) / 6] = 0;
}

bool WebLoginSessionIdIsSet()
{
    return sessionId[0];
}
void WebLoginSessionIdReset()
{
    sessionId[0] = 0;
}
char* WebLoginSessionIdGet()
{   
    return sessionId;
}
