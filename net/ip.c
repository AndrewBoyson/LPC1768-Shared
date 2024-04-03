#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "log/log.h"
#include "ip.h"

void IpProtocolString(uint8_t protocol, int size, char* text)
{
    switch (protocol)
    {
        case   ICMP: strncpy(text, "ICMP"  , size);       break;
        case   IGMP: strncpy(text, "IGMP"  , size);       break;
        case  ICMP6: strncpy(text, "ICMP6" , size);       break;
        case    TCP: strncpy(text, "TCP"   , size);       break;
        case    UDP: strncpy(text, "UDP"   , size);       break;
        case IP6IN4: strncpy(text, "IP6IN4", size);       break;
        default:    snprintf(text, size, "%d", protocol); break;
    }
}
void IpProtocolLog(uint8_t protocol)
{
    switch (protocol)
    {
        case   ICMP: Log("ICMP"  );       break;
        case   IGMP: Log("IGMP"  );       break;
        case  ICMP6: Log("ICMP6" );       break;
        case    TCP: Log("TCP"   );       break;
        case    UDP: Log("UDP"   );       break;
        case IP6IN4: Log("IP6IN4");       break;
        default:    LogF("%d", protocol); break;
    }
}
