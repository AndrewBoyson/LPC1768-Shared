#include <stdint.h>
#include <stdbool.h>

#include "clk/clktime.h"
#include "clk/clkutc.h"

#define SECONDS_BETWEEN_1900_AND_1970 2208988800ULL

#define ERA_BASE_LOW 1LL //Adjust this from 0 to 1 between 1968 and 2036; 1 to 2 between 2104 and 2168
#define ERA_BASE_HGH 0LL //Adjust this from 0 to 1 between 2036 and 2104; 1 to 2 between 2172 and 2240

uint64_t NtpTimeStampFromClkTime(clktime tai)
{
    clktime utc = ClkUtcFromTai(tai);
    uint64_t timestamp = utc << (32 - CLK_TIME_ONE_SECOND_SHIFT);
    
    timestamp += SECONDS_BETWEEN_1900_AND_1970 << 32; //This should just wrap around the unsigned timestamp removing the unwanted era.
    
    return timestamp;
}
clktime NtpTimeStampToClkTime(uint64_t timestamp)
{
    bool isHigh = (timestamp & (1ULL << 63)) != 0;
    
    clktime utc = timestamp - (SECONDS_BETWEEN_1900_AND_1970 << 32);
    utc >>= (32 - CLK_TIME_ONE_SECOND_SHIFT);
    
    //Correct for era
    if (isHigh) utc += ERA_BASE_HGH << (32 + CLK_TIME_ONE_SECOND_SHIFT);
    else        utc += ERA_BASE_LOW << (32 + CLK_TIME_ONE_SECOND_SHIFT);
    
    clktime tai = ClkUtcToTai(utc);
    
    return tai;
}

