#include "bitband.h"

#define DIV_VAL     0 //0 == do not use
#define MUL_VAL     1 //Cannot be zero

#define   U0RBR_ADDR 0x4000C000 //Receiver Buffer Register   (DLAB = 0 RO common RBR THR DLL)
#define   U0THR_ADDR 0x4000C000 //Transmit Holding Register  (DLAB = 0 WO common RBR THR DLL)
#define   U0DLL_ADDR 0x4000C000 //Divisor Latch LSB register (DLAB = 1 RW common RBR THR DLL)
#define   U0DLM_ADDR 0x4000C004 //Divisor Latch MSB register (DLAB = 1 RW common IER DLM)
#define   U0IER_ADDR 0x4000C004 //Divisor Latch MSB register (DLAB = 0 RW common IER DLM)
#define   U0FCR_ADDR 0x4000C008 //FIFO Control Register
#define   U0LCR_ADDR 0x4000C00C //Line Control Register
#define   U0LSR_ADDR 0x4000C014 //Line Status Register
#define   U0FDR_ADDR 0x4000C028 //Fractional Divider Register

#define RECEIVE_BUFFER_REGISTER     *((volatile unsigned *)   U0RBR_ADDR)
#define TRANSMIT_HOLDING_REGISTER   *((volatile unsigned *)   U0THR_ADDR)
#define LINE_CONTROL_REGISTER       *((volatile unsigned *)   U0LCR_ADDR)
#define DIVISOR_LSB                 *((volatile unsigned *)   U0DLL_ADDR)
#define DIVISOR_MSB                 *((volatile unsigned *)   U0DLM_ADDR)
#define FRACTIONAL_DIVIDER_REGISTER *((volatile unsigned *)   U0FDR_ADDR)

#define FIFO_ENABLE                     BIT_BAND4(U0FCR_ADDR,  0)
#define DIVISOR_ACCESS_BIT              BIT_BAND4(U0LCR_ADDR,  7) //Divisor Latch Access Bit
#define RECEIVER_DATA_READY             BIT_BAND4(U0LSR_ADDR,  0) //Receiver Data Ready
#define TRANSMIT_HOLDING_REGISTER_EMPTY BIT_BAND4(U0LSR_ADDR,  5) //Transmitter Holding Register Empty

int SerialPcGetC() // Returns a negative number if no character to read or if there was an error. 0 to 255 otherwise.
{
    if (!RECEIVER_DATA_READY) return -1;
    return RECEIVE_BUFFER_REGISTER;      //oldest character in the RX FIFO
}
int SerialPcPutC(char c) // Returns zero on success or -1 if the buffer was full or not ready
{
    if (!TRANSMIT_HOLDING_REGISTER_EMPTY) return -1; //set immediately upon detection of an empty THR and is cleared on a THR write.
    TRANSMIT_HOLDING_REGISTER = c;                   //newest character in the TX FIFO 
    return 0;
}

static void setBaud(int baud)
{
    int dl = (6000000 + (baud >> 1)) / baud; // 6,000,000 <== 96,000,000 / 16 ; adding 'baud >> 1' rounds the result to the nearest integer rather than rounding down.
    DIVISOR_ACCESS_BIT = 1;
        DIVISOR_MSB = dl >> 8;
        DIVISOR_LSB = dl & 0xFF;
        FRACTIONAL_DIVIDER_REGISTER = DIV_VAL | MUL_VAL << 4;
    DIVISOR_ACCESS_BIT = 0;
}
void SerialPcInit(int baud)
{            
    FIFO_ENABLE = 1;
    
    setBaud(baud);

    LINE_CONTROL_REGISTER |= 0x03; // 8 bit, 1 stop, no parity
}