#include <stdlib.h>

#include "lpc1768/tm/tm.h"
#include "http.h"
#include "clk/clk.h"

static void dateFromTm(struct tm* ptm, char* ptext)
{
/*If the length of the result string (including the terminating
  null byte) would exceed HTTP_DATE_LENGTH bytes, then strftime() returns 0, and
  the contents of the array are undefined. In this case we ensure ptext is an empty string.
*/
	size_t size = strftime(ptext, HTTP_DATE_LENGTH, "%a, %d %b %Y %H:%M:%S GMT", ptm);//Sun, 06 Nov 1994 08:49:37 GMT  ; RFC 822, updated by RFC 1123
	if (!size) *ptext = 0;
}
void HttpDateFromNow(char* pText)
{
    struct tm tm;
    ClkNowTmUtc(&tm);
    dateFromTm(&tm, pText);
}
void HttpDateFromDateTime(const char* date, const char *ptime, char* ptext)
{
    struct tm tm;
    TmFromAsciiDateTime(date, ptime, &tm);
    dateFromTm(&tm, ptext);
}
