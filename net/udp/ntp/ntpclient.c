#include <stdbool.h>

#include "ntpclient.h"
#include "clk/clkgov.h"

bool NtpClientTrace   = false;

uint64_t NtpClientQueryTime = 0;

void NtpClientTimeUpdateSuccessful()
{
    NtpClientQueryStartInterval(NTP_QUERY_INTERVAL_NORMAL);
    ClkGovIsReceivingTime = true;
}

void NtpClientInit()
{
    NtpClientQueryStartInterval(NTP_QUERY_INTERVAL_INITIAL);
    ClkGovIsReceivingTime = false;
}
