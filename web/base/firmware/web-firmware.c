#include "lpc1768/semihost.h"
#include "web/http/http.h"

void WebFirmwareListSemihostFiles()
{
    XFINFO info;
    info.fileID = 0;
    while (SemihostXffind ("*.*", &info) == 0)
    {
        HttpAddF("%13s %7d bytes\r\n", info.name, info.size);
    }
    if (info.fileID == 0)
    {
        HttpAddText("No files\r\n");
    }
}