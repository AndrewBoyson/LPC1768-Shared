#include <stdint.h>
#include <stdbool.h>

#include "lpc1768/firmware.h"

void WebFirmwarePost (int contentLength, int contentStart, int size, char* pRequestStream, uint32_t positionInRequestStream, bool* pComplete)
{
    if (!positionInRequestStream)
    {
        FirmwareStart(contentLength);
    }
    char* pDataStart  = pRequestStream + contentStart;
    int    dataLength =           size - contentStart;
    *pComplete = FirmwareAdd(pDataStart, dataLength);
}