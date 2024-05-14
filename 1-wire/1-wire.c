#include "log/log.h"
#include "1-wire-bus.h"
#include "1-wire.h"
#include "1-wire-device.h"
#include "lpc1768/hrtimer/hrtimer.h"

char OneWireInUse = 0;
char OneWireTrace = 0;

#define BUS_TIMEOUT_MS 20000

//Exchange zone
//=============
#define WAIT_FOR_SOMETHING_TO_DO     0
#define HANDLE_RESET_END             1
#define SEND_NEXT_XCHG_WRITE         2
#define HANDLE_XCHG_READ             3
#define SEND_NEXT_SEARCH_WRITE       4
#define SEND_NEXT_SEARCH_READ_BIT    5
#define HANDLE_SEARCH_READ_BIT_TRUE  6
#define HANDLE_SEARCH_READ_BIT_COMP  7
static int state = WAIT_FOR_SOMETHING_TO_DO;

#define JOB_NONE   0
#define JOB_XCHG   1
#define JOB_SEARCH 2
static int job = JOB_NONE;

static int  lensend = 0;
static int  lenrecv = 0;
static char*  psend = NULL;
static char*  precv = NULL;
static int pullupms = 0;

static char crc;
static void resetCrc()
{
    crc = 0;
}
static void addBitToCrc(int bit)
{
    int feedback = !(crc & 0x80) != !bit; //Logical Exclusive Or of the msb of the shift register with the input
    crc <<= 1;                            //Move the shift register one place to the left leaving a zero in the lsb and losing the msb
    if (feedback) crc ^= 0x31;            //Exclusive Or the shift register with polynomial X5 + X4 + 1
}

static void resetBitPosition(int* pByteIndex, char* pBitMask)
{
    *pBitMask = 1;
    *pByteIndex = 0;
}
static void incrementBitPosition(int* pByteIndex, char* pBitMask)
{
    *pBitMask <<= 1;
    if (!*pBitMask)
    {
        *pBitMask = 1;
        *pByteIndex += 1;
    }
}

int getBitAtPosition(int byteIndex, char bitMask)
{
    return psend[byteIndex] & bitMask;
}
void setBitAtPosition(int byteIndex, char bitMask, int bit)
{
    if ( bit) precv[byteIndex] |=  bitMask;
    else      precv[byteIndex] &= ~bitMask;
}
int moreBitsToRead(int byteIndex)
{
    return byteIndex < lenrecv;
}
int moreBitsToWrite(int byteIndex)
{
    return byteIndex < lensend;
}

int result = ONE_WIRE_RESULT_OK;
int OneWireResult()
{
    return result;
}
void OneWireInit(char* pin)
{
    OneWireBusInit(pin);
    DeviceInit();
    state = WAIT_FOR_SOMETHING_TO_DO;
    result = ONE_WIRE_RESULT_OK;
    job   =  JOB_NONE;
	OneWireInUse = 1;
}
bool OneWireBusy()
{
    return state;
}
void OneWireExchange(int lenBytesToSend, int lenBytesToRecv, char *pBytesToSend, char *pBytesToRecv, int msToPullUp)
{
    lensend = lenBytesToSend;
    lenrecv = lenBytesToRecv;
    psend = pBytesToSend;
    precv = pBytesToRecv;
    pullupms = msToPullUp;
    state = HANDLE_RESET_END;
    job = JOB_XCHG;
    OneWireBusReset();
}
static bool* pallfound;
static char* prom;
static void setRomBit(int position, int bit)
{
    int bitindex = position & 0x07;
    int byteindex = position >> 3;
    int bitmask = 1 << bitindex;
    if (bit) *(prom + byteindex) |=  bitmask;
    else     *(prom + byteindex) &= ~bitmask;
}
static int getRomBit(int position)
{    
    int bitindex = position & 0x07;
    int byteindex = position >> 3;
    int bitmask = 1 << bitindex;
    return *(prom + byteindex) & bitmask;
}
static int thisFurthestForkLeftPosn;
static int lastFurthestForkLeftPosn;
static char searchCommand;
static int searchBitPosn;
static int searchBitTrue;
static int searchBitComp;
static int chooseDirectionToTake()
{
    if ( searchBitTrue &&  searchBitComp) return -1; //No devices are participating in the search
    if ( searchBitTrue && !searchBitComp) return  1; //Only devices with a one  at this point are participating
    if (!searchBitTrue &&  searchBitComp) return  0; //Only devices with a zero at this point are participating
    //Both bits are zero so devices with both 0s and 1s at this point are still participating
    
    //If we have not yet reached the furthest away point we forked left (0) last time then just do whatever we did last time
    if (searchBitPosn <  lastFurthestForkLeftPosn)
    {
        int romBit = getRomBit(searchBitPosn);
        if (OneWireTrace) LogF("Bit %d:%d Last rom forked further on so follow it.", searchBitPosn, romBit);
        if (romBit == 0)
        {
            thisFurthestForkLeftPosn = searchBitPosn;
            if (OneWireTrace) LogF(" Record furthest left fork.");
        }
        if (OneWireTrace) LogF("\r\n");
        return romBit;
    }
    
    //If we are at the furthest away point that we forked left (0) last time then this time fork right (1)
    if (searchBitPosn == lastFurthestForkLeftPosn)
    {
        if (OneWireTrace) LogF("Bit %d:1 Last rom forked left here so this time fork right.\r\n");
        return 1;
    }
    
    //We are at a new fork point further than we have been before so fork left (0) and record that we did so.
    thisFurthestForkLeftPosn = searchBitPosn;
    if (OneWireTrace) LogF("Bit %d:0 New fork so fork left. Record furthest left fork.\r\n", searchBitPosn);
    return 0; 
}
void OneWireSearch(char command, char* pDeviceRom, bool* pAllDevicesFound) //Specify the buffer to receive the rom for the first search and NULL thereafter.
{
    if (pDeviceRom)
    {
        if (OneWireTrace) Log("\r\n");
        if (OneWireTrace) Log("Search for first device\r\n");
        pallfound = pAllDevicesFound;
        *pallfound = false;
        lastFurthestForkLeftPosn = -1;
        prom = pDeviceRom;
        for (int i = 0; i < 8; i++) *(prom + i) = 0;
    }
    else
    {
        if (OneWireTrace) Log("Search for next device\r\n");
    }
    thisFurthestForkLeftPosn = -1;
    lensend = 1;
    lenrecv = 0;
    searchCommand = command;
    psend = &searchCommand;
    precv = NULL;
    pullupms = 0;
    job = JOB_SEARCH;
    state = HANDLE_RESET_END;
    OneWireBusReset();
}
char OneWireCrc()
{
    return crc;
}
static void handleState()
{
    static int byteindex;
    static char bitmask;
    static uint32_t busTimer;
        
    if (state)
    {
        int elapsedMs = HrTimerSinceMs(busTimer);
        if (elapsedMs > BUS_TIMEOUT_MS)
        {
            LogTime("1-wire bus timed out so protocol has been reset to idle.\r\n");
            OneWireInit(0);
            result = ONE_WIRE_RESULT_TIMED_OUT;
            return;
        }
    }
    else
    {
        busTimer = HrTimerCount();
    }

    if (OneWireBusBusy()) return;
    
    switch(state)
    {
        case WAIT_FOR_SOMETHING_TO_DO:
            break;
        case HANDLE_RESET_END:
            if (OneWireBusValue)
            {
                if (OneWireTrace) LogTime("No 1-wire device presence detected on the bus\r\n");
                result = ONE_WIRE_RESULT_NO_DEVICE_PRESENT;
                state = WAIT_FOR_SOMETHING_TO_DO;
            }
            else
            {
                resetBitPosition(&byteindex, &bitmask);
                switch (job)
                {
                    case JOB_XCHG:   state =   SEND_NEXT_XCHG_WRITE; break;
                    case JOB_SEARCH: state = SEND_NEXT_SEARCH_WRITE; break;
                    default:
                        LogTimeF("Unknown job in RESET_RELEASE %d\r\n", job);
                        break;
                }
            }
            break;
            
        case SEND_NEXT_XCHG_WRITE:
            if (moreBitsToWrite(byteindex))
            {
                int bit = getBitAtPosition(byteindex, bitmask);
                incrementBitPosition(&byteindex, &bitmask);
                if (moreBitsToWrite(byteindex)) OneWireBusWriteBit(bit);
                else                            OneWireBusWriteBitWithPullUp(bit, pullupms);
            }
            else
            {
                resetBitPosition(&byteindex, &bitmask);
                if (moreBitsToRead(byteindex))
                {
                    resetCrc();
                    OneWireBusReadBit();
                    state = HANDLE_XCHG_READ;
                }
                else
                {
                    result = ONE_WIRE_RESULT_OK;
                    state = WAIT_FOR_SOMETHING_TO_DO;
                }
            }
            break;
        case HANDLE_XCHG_READ:
            addBitToCrc(OneWireBusValue);
            setBitAtPosition(byteindex, bitmask, OneWireBusValue);
            incrementBitPosition(&byteindex, &bitmask);
            if (moreBitsToRead(byteindex))
            {
                OneWireBusReadBit();
            }
            else
            {
                state = WAIT_FOR_SOMETHING_TO_DO;
                result = crc ? ONE_WIRE_RESULT_CRC_ERROR : ONE_WIRE_RESULT_OK;
            }
            break;
            
        case SEND_NEXT_SEARCH_WRITE:
            if (moreBitsToWrite(byteindex))
            {
                int bit = getBitAtPosition(byteindex, bitmask);
                incrementBitPosition(&byteindex, &bitmask);
                OneWireBusWriteBit(bit);
            }
            else
            {
                searchBitPosn = 0;
                state = SEND_NEXT_SEARCH_READ_BIT;
            }
            break;
        case SEND_NEXT_SEARCH_READ_BIT: //Have to have this extra step to separate from action in HANDLE_SEARCH_READ_BIT_COMP
            OneWireBusReadBit();
            state = HANDLE_SEARCH_READ_BIT_TRUE;
            break;
        case HANDLE_SEARCH_READ_BIT_TRUE:
            searchBitTrue = OneWireBusValue;
            OneWireBusReadBit();
            state = HANDLE_SEARCH_READ_BIT_COMP;
            break;
        case HANDLE_SEARCH_READ_BIT_COMP:
            searchBitComp = OneWireBusValue;
            int direction;
            direction = chooseDirectionToTake();
            if (direction == -1)
            {
                if (OneWireTrace) LogTime("No devices have responded\r\n");
                result = ONE_WIRE_RESULT_NO_DEVICE_PARTICIPATING;
                state = WAIT_FOR_SOMETHING_TO_DO;
            }
            else
            {
                setRomBit(searchBitPosn, direction);
                OneWireBusWriteBit(direction);
                searchBitPosn++;
                if (searchBitPosn < 64)
                {
                    state = SEND_NEXT_SEARCH_READ_BIT;
                }
                else
                {
                    if (thisFurthestForkLeftPosn == -1)
                    {
                         if (OneWireTrace) Log("All devices found\r\n");
                         *pallfound = 1;
                    }
                    lastFurthestForkLeftPosn = thisFurthestForkLeftPosn;
                    result = ONE_WIRE_RESULT_OK;
                    state = WAIT_FOR_SOMETHING_TO_DO;
                }
            }
            break;
        default:
            LogTimeF("Unknown state %d\r\n", state);
            break;
    }
}
void OneWireMain()
{
	handleState();
	DeviceMain();
}
