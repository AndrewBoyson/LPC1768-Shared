#include <stdint.h>
#include <stdbool.h>

#include "i2c1.h"
#include "log/log.h"
#include "lpc1768/mstimer/mstimer.h"

#define I2C1SCLH   *((volatile unsigned *)0x4005C010) // High duty cycle register
#define I2C1SCLL   *((volatile unsigned *)0x4005C014) // Low  duty cycle register
#define I2C1CONSET *((volatile unsigned *)0x4005C000) // Control Set register
#define I2C1CONCLR *((volatile unsigned *)0x4005C018) // Control Clear register
#define I2C1STAT   *((volatile unsigned *)0x4005C004) // Status register
#define I2C1DAT    *((volatile unsigned *)0x4005C008) // Data register

#define TIMEOUT_MS 200

bool I2C1Trace = false;

bool I2C1Busy = true;

uint8_t  _addr;
uint8_t* _pData;
int      _length;
int*     _pResult;
uint32_t _msTimeOut;

static void logStatus(uint8_t stat)
{
    switch (stat)
    {
        case 0x08: Log ("START transmitted"         ); break;
        case 0x10: Log ("repeat START transmitted"  ); break;
        case 0x18: Log ("received ACK after address"); break;
        case 0x20: Log ("received NAK after address"); break;
        case 0x28: Log ("received ACK after data"   ); break;
        case 0x30: Log ("received NAK after data"   ); break;
        case 0x38: Log ("arbitration lost"          ); break;
        default:   LogF("unknown status %02x"       ); break;
    }
}

void I2C1Init()
{
    
    // I2Cbitfrequency = PCLKI2C / (I2CSCLH + I2CSCLL)
    // Where PCLKI2C == 96MHz as the divider is set to 1 in periphs.c
    // We want 100 kHz for standard I2C
    // I2CSCLH + I2CSCLL = 96000 / 100
    // I2CSCLH = I2CSCLL = 480
    I2C1SCLH = 480;
    I2C1SCLL = 480;
    I2C1CONCLR = 0xFF; //Clear the control register
    I2C1CONSET = 0x40; //Set I2EN bit
    I2C1Busy = false;
}
void I2C1Main()
{
    static int index = 0;
    
    if (I2C1Busy && MsTimerRelative(_msTimeOut, TIMEOUT_MS))
    {
        if (I2C1Trace) LogTime("I2C1 -> Timeout\r\n");
        *_pResult  = I2C1_RESULT_TIMED_OUT;
        I2C1Busy   = false;
        I2C1CONSET = 0x10;          //Set STO bit
        I2C1CONCLR = 0x08;          //Clear S1
        return;
    }
    
    if (I2C1CONSET & 0x08) //S1 is set
    {
        if (I2C1Trace) { LogTime("I2C1Main <- Status '"); logStatus(I2C1STAT); Log("'\r\n"); }
        switch (I2C1STAT)
        {
            case 0x08: //Received START
            case 0x10: //Received a repeated START
                if (I2C1Trace) LogTimeF("I2C1 -> addr %02x + W\r\n", _addr);
                I2C1DAT = (_addr << 1) | 0; //Send the address + W
                I2C1CONCLR = 0x08;          //Clear S1
                index = 0;
                break;
            case 0x18: //received ACK after address
                if (I2C1Trace) LogTimeF("I2C1 -> data%d %02x\r\n", index, _pData[index]);
                I2C1DAT = _pData[index];    //Load the data to be sent
                I2C1CONCLR = 0x20;          //Reset the start bit
                I2C1CONCLR = 0x08;          //Clear S1
                break;
            case 0x20: //received NAK after address
                if (I2C1Trace) LogTime("I2C1 -> Stop\r\n");
                *_pResult  = I2C1_RESULT_FAILED;
                I2C1Busy   = false;
                I2C1CONSET = 0x10;          //Set STO bit
                I2C1CONCLR = 0x08;          //Clear S1
                break;
            case 0x28: //received ACK after data
                index++;
                if (index >= _length)
                {
                    if (I2C1Trace) LogTime("I2C1 -> Stop\r\n");
                    *_pResult  = I2C1_RESULT_OK;
                     I2C1Busy  = false;
                    I2C1CONSET = 0x10;      //Set STO bit
                }
                else
                {
                    if (I2C1Trace) LogTimeF("I2C1 -> data%d %02x\r\n", index, _pData[index]);
                    I2C1DAT = _pData[index];//Load the data to be sent
                }
                I2C1CONCLR = 0x08;          //Clear S1
                break;
            case 0x30: //received NAK after data
                if (I2C1Trace) LogTime("I2C1 -> Stop\r\n");
                *_pResult  = I2C1_RESULT_FAILED;
                I2C1Busy   = false;
                I2C1CONSET = 0x10;          //Set STO bit
                I2C1CONCLR = 0x08;          //Clear S1
                break;
            case 0x38: //Arbitration lost
                if (I2C1Trace) LogTime("I2C1 -> Start\r\n");
                *_pResult  = I2C1_RESULT_FAILED;
                I2C1Busy   = false;
                I2C1CONSET = 0x20;          //Set STA bit
                I2C1CONCLR = 0x08;          //Clear S1
                break;
            default:
                LogTime("I2C1Main <- Status '"); logStatus(I2C1STAT); Log("'\r\n");
                break;
        }
    }
}
void I2C1Send(uint8_t addr, int length, uint8_t* pData, int* pResult)
{
    if (I2C1Trace) { LogTime("I2C1 -> Start\r\n"); }
    _addr      = addr;
    _length    = length;
    _pData     = pData;
    _pResult   = pResult;
    *_pResult  = I2C1_RESULT_NONE;
    _msTimeOut = MsTimerCount;
    I2C1Busy   = true;
    I2C1CONSET = 0x20;          //Set STA bit
    I2C1CONCLR = 0x08;          //Clear S1
}
uint8_t I2C1GetData()
{
    return I2C1DAT;
}