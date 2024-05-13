#include "web/web-add.h"
#include "web-nav-base.h"
#include "1-wire/1-wire.h"

void WebNavBase(int page)
{
	if (OneWireInUse) WebAddNavItem(page == ONE_WIRE_PAGE, "/1wire",    "1-Wire"    );
					  WebAddNavItem(page ==    CLOCK_PAGE, "/clock",    "Clock"     );
					  WebAddNavItem(page ==    RESET_PAGE, "/reset",    "Last Reset");
					  WebAddNavItem(page ==      NET_PAGE, "/net",      "Net"       );
					  WebAddNavItem(page ==     NET4_PAGE, "/net4",     "Net IPv4"  );
					  WebAddNavItem(page ==     NET6_PAGE, "/net6",     "Net IPv6"  );
					  WebAddNavItem(page ==    TRACE_PAGE, "/trace",    "Net Trace" );
				 	  WebAddNavItem(page ==      LOG_PAGE, "/log",      "Log"       );
					  WebAddNavItem(page == FIRMWARE_PAGE, "/firmware", "Firmware"  );
}