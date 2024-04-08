#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "log/log.h"
#include "mstimer/mstimer.h"
#include "semihost.h"
#include "gpio.h"

#define TIMOUT_MS 1000

bool     FirmwareTrace = false;

int      FirmwareSentLength   = 0;
int      FirmwareRcvdLength   = 0;
int      FirmwareFileLength   = 0;

bool     FirmwareFailed = false;

static char       filename[13];
static FILEHANDLE fh = 0;
static uint32_t   msTimerTimeout = 0;


//All these routines abort immediately if a previous operation has failed
static void deleteBinFiles()
{
    if (FirmwareFailed) return;
    XFINFO info;
    info.fileID = 0;
    while (SemihostXffind("*.BIN", &info) == 0)
    {
        int r =  SemihostRemove(info.name); //0 on success
        if (r)
        {
            LogTimeF("Firmware - error deleting file %s\r\n", info.name);
            FirmwareFailed = true;
        }
    }
}
static void createUniqueName()
{
    if (FirmwareFailed) return;
    if (sprintf(filename, "%08lX.BIN", MsTimerCount) < 0)
    {
        FirmwareFailed = true;
    }
}
static void __attribute__ ((unused)) openFileWrite()
{
    if (FirmwareFailed) return;
    if (FirmwareTrace) LogTimeF("Firmware - open file write %s\r\n", filename);
    fh = SemihostOpen(filename, OPEN_MODE_W + OPEN_MODE_B);
    if (fh <= 0)
    {
        LogTimeF("Firmware - open file for write error\r\n");
        FirmwareFailed = true;
    }

    msTimerTimeout = MsTimerCount;
}
static void openFileAppend()
{
    if (FirmwareFailed) return;
    if (FirmwareTrace) LogTimeF("Firmware - open file append %s\r\n", filename);
    fh = SemihostOpen(filename, OPEN_MODE_A + OPEN_MODE_B);
    if (fh <= 0)
    {
        LogTimeF("Firmware - open file for append error\r\n");
        FirmwareFailed = true;
    }

    msTimerTimeout = MsTimerCount;
}
static void openFileRead()
{
    if (FirmwareFailed) return;
    if (FirmwareTrace) LogTimeF("Firmware - open file read %s\r\n", filename);
    fh = SemihostOpen(filename, OPEN_MODE_R + OPEN_MODE_B);
    if (fh <= 0)
    {
        LogTimeF("Firmware - open file for read error\r\n");
        FirmwareFailed = true;
    }

    msTimerTimeout = MsTimerCount;
}
static void writeFile(const char* buffer, int length)
{
    if (FirmwareFailed) return;
    if (FirmwareTrace) LogTimeF("Firmware - write: data length %d, accumulated length %d, target length %d\r\n", length, FirmwareRcvdLength, FirmwareSentLength);
    int written = SemihostWrite(fh, buffer, length);
    if (written < length)
    {
        LogTimeF("Firmware - write file error\r\n");
        FirmwareFailed = true;
        SemihostClose(fh);
        fh = 0;
    }
    
    msTimerTimeout = MsTimerCount;
}
static void getLengthFile()
{
    if (FirmwareFailed) return;
    int r = SemihostFlen(fh);
    if (FirmwareTrace) LogTimeF("Firmware - read length %d bytes\r\n", r);
    if (r < 0)
    {
        LogTimeF("Firmware - file length error\r\n");
        FirmwareFileLength = 0;
        FirmwareFailed = true;
        SemihostClose(fh);
        fh = 0;
    }
    FirmwareFileLength = r;
}
static void closeFile()
{
    if (FirmwareFailed) return;
    if (FirmwareTrace) LogTimeF("Firmware - close file\r\n");
    int r = SemihostClose(fh); //0 on success; -1 on failure
    if (r)
    {
        LogTimeF("Firmware - close file error\r\n");
        FirmwareFailed = true;
    }
    fh = 0;
}
void FirmwareStart(int contentLength)
{
    FirmwareSentLength   = contentLength;
    FirmwareRcvdLength   = 0;
    FirmwareFileLength   = 0;
    FirmwareFailed = false;
    deleteBinFiles();
    createUniqueName();
}
bool FirmwareAdd(const char* pDataStart, int dataLength)
{
    FirmwareRcvdLength += dataLength;
    
    openFileAppend();
    writeFile(pDataStart, dataLength);
    closeFile();
    bool postComplete = FirmwareRcvdLength >= FirmwareSentLength;
    if (postComplete)
    {
        openFileRead();
        getLengthFile();
        closeFile();
    }

    return postComplete;
}
void FirmwareMain()
{
    if (fh && MsTimerRelative(msTimerTimeout, TIMOUT_MS)) closeFile();
}