#include "web/http/http.h"
#include "web/web.h"
#include "web-pages-base.h"
#include "1-wire/1-wire.h"

#define DO_FAVICON         DO_BASE +  1
#define DO_BASE_CSS        DO_BASE +  2
#define DO_NAV_CSS         DO_BASE +  3
#define DO_1WIRE_HTML      DO_BASE +  4
#define DO_1WIRE_AJAX      DO_BASE +  5
#define DO_1WIRE_SCRIPT    DO_BASE +  6
#define DO_CLOCK_HTML      DO_BASE +  7
#define DO_CLOCK_AJAX      DO_BASE +  8
#define DO_CLOCK_SCRIPT    DO_BASE +  9
#define DO_NET_HTML        DO_BASE + 10
#define DO_NET_AJAX        DO_BASE + 11
#define DO_NET_SCRIPT      DO_BASE + 12
#define DO_NET4_HTML       DO_BASE + 13
#define DO_NET4_AJAX       DO_BASE + 14
#define DO_NET4_SCRIPT     DO_BASE + 15
#define DO_NET6_HTML       DO_BASE + 16
#define DO_NET6_AJAX       DO_BASE + 17
#define DO_NET6_SCRIPT     DO_BASE + 18
#define DO_TRACE_HTML      DO_BASE + 19
#define DO_TRACE_AJAX      DO_BASE + 20
#define DO_TRACE_SCRIPT    DO_BASE + 21
#define DO_LOG_HTML        DO_BASE + 22
#define DO_RESET_HTML      DO_BASE + 23
#define DO_FIRMWARE_HTML   DO_BASE + 24
#define DO_FIRMWARE_AJAX   DO_BASE + 25
#define DO_FIRMWARE_SCRIPT DO_BASE + 26

int WebServerBaseDecideWhatToDo(char *pPath, char* pLastModified)
{
    if (OneWireInUse && HttpSameStr(pPath, "/1wire"        )) return DO_1WIRE_HTML;
    if (OneWireInUse && HttpSameStr(pPath, "/1wire-ajax"   )) return DO_1WIRE_AJAX;
    if (                HttpSameStr(pPath, "/clock"        )) return DO_CLOCK_HTML;
    if (                HttpSameStr(pPath, "/clock-ajax"   )) return DO_CLOCK_AJAX;
    if (                HttpSameStr(pPath, "/net"          )) return DO_NET_HTML;
    if (                HttpSameStr(pPath, "/net-ajax"     )) return DO_NET_AJAX;
    if (                HttpSameStr(pPath, "/net4"         )) return DO_NET4_HTML;
    if (                HttpSameStr(pPath, "/net4-ajax"    )) return DO_NET4_AJAX;
    if (                HttpSameStr(pPath, "/net6"         )) return DO_NET6_HTML;
    if (                HttpSameStr(pPath, "/net6-ajax"    )) return DO_NET6_AJAX;
    if (                HttpSameStr(pPath, "/log"          )) return DO_LOG_HTML;
    if (                HttpSameStr(pPath, "/trace"        )) return DO_TRACE_HTML;
    if (                HttpSameStr(pPath, "/trace-ajax"   )) return DO_TRACE_AJAX;
    if (                HttpSameStr(pPath, "/reset"        )) return DO_RESET_HTML;
    if (                HttpSameStr(pPath, "/firmware"     )) return DO_FIRMWARE_HTML;
    if (                HttpSameStr(pPath, "/firmware-ajax")) return DO_FIRMWARE_AJAX;
    
    if (                HttpSameStr(pPath, "/favicon.ico"  )) return HttpSameDate(WebFaviconDate,        WebFaviconTime,        pLastModified) ? DO_NOT_MODIFIED : DO_FAVICON;
    if (                HttpSameStr(pPath, "/base.css"     )) return HttpSameDate(WebBaseCssDate,        WebBaseCssTime,        pLastModified) ? DO_NOT_MODIFIED : DO_BASE_CSS;
    if (                HttpSameStr(pPath, "/settings.css" )) return HttpSameDate(WebNavCssDate,         WebNavCssTime,         pLastModified) ? DO_NOT_MODIFIED : DO_NAV_CSS;
    if (                HttpSameStr(pPath, "/net.js"       )) return HttpSameDate(WebNetScriptDate,      WebNetScriptTime,      pLastModified) ? DO_NOT_MODIFIED : DO_NET_SCRIPT;
    if (                HttpSameStr(pPath, "/net4.js"      )) return HttpSameDate(WebNet4ScriptDate,     WebNet4ScriptTime,     pLastModified) ? DO_NOT_MODIFIED : DO_NET4_SCRIPT;
    if (                HttpSameStr(pPath, "/net6.js"      )) return HttpSameDate(WebNet6ScriptDate,     WebNet6ScriptTime,     pLastModified) ? DO_NOT_MODIFIED : DO_NET6_SCRIPT;
    if (                HttpSameStr(pPath, "/trace.js"     )) return HttpSameDate(WebTraceScriptDate,    WebTraceScriptTime,    pLastModified) ? DO_NOT_MODIFIED : DO_TRACE_SCRIPT;
    if (OneWireInUse && HttpSameStr(pPath, "/1wire.js"     )) return HttpSameDate(WebOneWireScriptDate,  WebOneWireScriptTime,  pLastModified) ? DO_NOT_MODIFIED : DO_1WIRE_SCRIPT;
    if (                HttpSameStr(pPath, "/clock.js"     )) return HttpSameDate(WebClockScriptDate,    WebClockScriptTime,    pLastModified) ? DO_NOT_MODIFIED : DO_CLOCK_SCRIPT;
    if (                HttpSameStr(pPath, "/firmware.js"  )) return HttpSameDate(WebFirmwareScriptDate, WebFirmwareScriptTime, pLastModified) ? DO_NOT_MODIFIED : DO_FIRMWARE_SCRIPT;

    return DO_NOT_FOUND;
}

bool WebServerBaseHandleQuery(int todo, char* pQuery)
{
    switch (todo)
    {
        case DO_TRACE_AJAX:    WebTraceQuery   (pQuery); return true;
        case DO_1WIRE_HTML:    WebOneWireQuery (pQuery); return true;
        case DO_1WIRE_AJAX:    WebOneWireQuery (pQuery); return true;
        case DO_CLOCK_AJAX:    WebClockQuery   (pQuery); return true;
        case DO_CLOCK_HTML:    WebClockQuery   (pQuery); return true;
        case DO_NET_AJAX:      WebNetQuery     (pQuery); return true;
        case DO_LOG_HTML:      WebLogQuery     (pQuery); return true;
        case DO_RESET_HTML:    WebResetQuery   (pQuery); return true;
        case DO_FIRMWARE_HTML: WebFirmwareQuery(pQuery); return true;
        case DO_FIRMWARE_AJAX: WebFirmwareQuery(pQuery); return true;
    }
    return false;
}
bool WebServerBasePost(int todo, int contentLength, int contentStart, int size, char* pRequestStream, uint32_t positionInRequestStream, bool* pComplete)
{
    switch (todo)
    {
        case DO_FIRMWARE_AJAX: WebFirmwarePost(contentLength, contentStart, size, pRequestStream, positionInRequestStream, pComplete); return true;
    }
    return false;
}
bool WebServerBaseReply(int todo)
{
    switch (todo)
    {
        case DO_FAVICON:         WebFavicon       (); return true;
        case DO_BASE_CSS:        WebBaseCss       (); return true;
        case DO_NAV_CSS:         WebNavCss        (); return true;
        case DO_TRACE_HTML:      WebTraceHtml     (); return true;
        case DO_TRACE_AJAX:      WebTraceAjax     (); return true;
        case DO_TRACE_SCRIPT:    WebTraceScript   (); return true;
        case DO_1WIRE_HTML:      WebOneWireHtml   (); return true;
        case DO_1WIRE_AJAX:      WebOneWireAjax   (); return true;
        case DO_1WIRE_SCRIPT:    WebOneWireScript (); return true;
        case DO_CLOCK_HTML:      WebClockHtml     (); return true;
        case DO_CLOCK_AJAX:      WebClockAjax     (); return true;
        case DO_CLOCK_SCRIPT:    WebClockScript   (); return true;
        case DO_NET_HTML:        WebNetHtml       (); return true;
        case DO_NET_AJAX:        WebNetAjax       (); return true;
        case DO_NET_SCRIPT:      WebNetScript     (); return true;
        case DO_NET4_HTML:       WebNet4Html      (); return true;
        case DO_NET4_AJAX:       WebNet4Ajax      (); return true;
        case DO_NET4_SCRIPT:     WebNet4Script    (); return true;
        case DO_NET6_HTML:       WebNet6Html      (); return true;
        case DO_NET6_AJAX:       WebNet6Ajax      (); return true;
        case DO_NET6_SCRIPT:     WebNet6Script    (); return true;
        case DO_LOG_HTML:        WebLogHtml       (); return true;
        case DO_RESET_HTML:      WebResetHtml     (); return true;
        case DO_FIRMWARE_HTML:   WebFirmwareHtml  (); return true;
        case DO_FIRMWARE_AJAX:   WebFirmwareAjax  (); return true;
        case DO_FIRMWARE_SCRIPT: WebFirmwareScript(); return true;
    }
    return false;
}
