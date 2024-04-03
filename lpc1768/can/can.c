
#include "log/log.h"
#include "lpc1768/mstimer/mstimer.h"

#define AFMR      *((volatile unsigned *)0x4003C000) // Acceptance Filter Mode Register

#define CAN2MOD   *((volatile unsigned *)0x40048000) // CAN Mode register
#define CAN2CMR   *((volatile unsigned *)0x40048004) // CAN Command register
#define CAN2GSR   *((volatile unsigned *)0x40048008) // CAN Global Status register
#define CAN2ICR   *((volatile unsigned *)0x4004800C) // CAN Interrupt and Capture Register
#define CAN2IER   *((volatile unsigned *)0x40048010) // CAN Interrupt Enable Register
#define CAN2BTR   *((volatile unsigned *)0x40048014) // CAN Bus Timing Register
#define CAN2TFI1  *((volatile unsigned *)0x40048030) // CAN Transmit Frame Information register 
#define CAN2TID1  *((volatile unsigned *)0x40048034) // CAN Transmit Identifier Register
#define CAN2TDA1  *((volatile unsigned *)0x40048038) // CAN Transmit Data Register A
#define CAN2TDB1  *((volatile unsigned *)0x4004803C) // CAN Transmit Data Register B
#define CAN2RFS   *((volatile unsigned *)0x40048020) // CAN Receive Frame Status register
#define CAN2RID   *((volatile unsigned *)0x40048024) // CAN Receive Identifier register
#define CAN2RDA   *((volatile unsigned *)0x40048028) // CAN Receive Data register A
#define CAN2RDB   *((volatile unsigned *)0x4004802C) // CAN Receive Data register B

void (*CanReceive)(uint16_t id, int length, uint32_t dataA, uint32_t dataB);
void CanInit()
{
    LogTime("Can init started\r\n");
    /*
    00:   BRP = 95 : Clock is 96 / 4 = 24MHz and we want 1.000MHz so prescale by BRP+1 = 24: BRP = 23.
    14:   SJW =  3 : 4 can clocks
    16: TESG1 =  4 : 5 can clocks
    20: TESG2 =  3 : 4 can clocks
    */
    CAN2BTR  = 0;
    CAN2BTR |= 23 << 00;
    CAN2BTR |=  3 << 14;
    CAN2BTR |=  4 << 16;
    CAN2BTR |=  3 << 20;
    
    AFMR |= 1 << 1; //Accept all messages
    
    CAN2MOD = 0; //Controller to operating mode
}
void CanSend(uint16_t id, int length, uint32_t dataA, uint32_t dataB)
{
    if (length > 8) length = 8;
    CAN2CMR |= 1 << 5; //STB1 Select Tx Buffer 1
    CAN2TFI1 = 0;
    CAN2TFI1 |= length << 16; //DLC Data Length Code
    CAN2TFI1 |= 0 << 30; //RTR Remote TRansmission
    CAN2TFI1 |= 0 << 31; // FF Extended frame (0 = 11 bit id; 1 = 29 bit id)
    CAN2TID1 = id;
    CAN2TDA1 = dataA;
    CAN2TDB1 = dataB;
    CAN2CMR |= 1 << 0; //TR Transmission Request
}
void CanMain()
{
    if (CAN2GSR & 0x01) //RBS Receive Buffer Status - At least one complete message is  available in CANxRFS CANxRID CANxRDA CANxRDB
    {
        uint16_t id = CAN2RID & 0x3FF;
        int      length = (CAN2RFS >> 16) & 0xF;
        CanReceive(id, length, CAN2RDA, CAN2RDB);
        CAN2CMR |= 1 << 2; //RRB Release Receive Buffer
    }
}