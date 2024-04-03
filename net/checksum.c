#include <stdint.h>

/*
Checksum is calculated on 2 byte chunks
All the routines below rely on the start being at a 2 byte boundary
It makes no sence for any, other than the last, call to accumulate the checksum to have an odd number of bytes

Start the first call with a sum of zero.
Thereafter propagate it to each subsequent call.
Always call the Fin routine for the last call to roll back in any carries, to convert to 16 bit and to take the ones complement.
The Fin call can have an odd number of byes but cannot invert.

The simple CheckSum is for when there is just one lump of data to calculate the checksum over.

If the lump of data starts at an odd address the 2 byte cast will result in an unaligned access.
This is ok for the LPC1768 M3, it just takes it a bit longer, but could cause other processors to fault.
It should never happen as no protocol is not at least aligned on even bytes and the buffers all start aligned.
*/

uint32_t CheckSumAddDirect(uint32_t sum, int count, void* pData)
{    
    uint16_t* p = (uint16_t*)pData;                     //Set up a 16 bit pointer for the data
    
    while(count > 1)
    {
        sum += *p++;                                    // Add each pair of bytes into 32 bit accumulator
        count -= 2;
    }
    if (count) sum += *(uint8_t*) p;                    // Add left-over byte, if any
    return sum;
}
uint32_t CheckSumAddInvert(uint32_t sum, int count, void* pData)
{
    uint16_t* p = (uint16_t*)pData;                     //Set up a 16 bit pointer for the data
    
    while(count > 1)
    {
        uint16_t value  = (*p & 0xFF00) >> 8;           // Invert the value
                 value |= (*p & 0x00FF) << 8;
        p++;
        sum += value;                                   // Add each pair of bytes into 32 bit accumulator
        count -= 2;
    }
    if (count) sum += *(uint8_t*) p;                    // Add left-over byte, if any
    return sum;
}
uint16_t CheckSumFinDirect(uint32_t sum, int count, void* pData)
{
    sum = CheckSumAddDirect(sum, count, pData);
    while (sum>>16) sum = (sum & 0xffff) + (sum >> 16); // Add any carries from the sum back into the sum to make it ones complement
    return ~sum;
}
uint16_t CheckSum(int count, void* pData)
{
    return CheckSumFinDirect(0, count, pData);
}
