#include "web/web-add.h"
#include "web-nav-base.h"

void WebNavBase(int page)
{
    WebAddNavItem(page ==    CLOCK_PAGE, "/clock",    "Clock"     );
    WebAddNavItem(page ==    RESET_PAGE, "/reset",    "Last Reset");
    WebAddNavItem(page ==      NET_PAGE, "/net",      "Net"       );
    WebAddNavItem(page ==     NET4_PAGE, "/net4",     "Net IPv4"  );
    WebAddNavItem(page ==     NET6_PAGE, "/net6",     "Net IPv6"  );
    WebAddNavItem(page ==    TRACE_PAGE, "/trace",    "Net Trace" );
    WebAddNavItem(page ==      LOG_PAGE, "/log",      "Log"       );
    WebAddNavItem(page == FIRMWARE_PAGE, "/firmware", "Firmware"  );
}