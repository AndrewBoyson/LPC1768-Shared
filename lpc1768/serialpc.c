#include "register.h"
#include "bitband.h"

#define DIV_VAL     0 //0 == do not use
#define MUL_VAL     1 //Cannot be zero

int SerialPcGetC() // Returns a negative number if no character to read or if there was an error. 0 to 255 otherwise.
{
    if (!U0_RECEIVER_DATA_READY) return -1;
    return U0_RECEIVE_BUFFER_REGISTER;      //oldest character in the RX FIFO
}
int SerialPcPutC(char c) // Returns zero on success or -1 if the buffer was full or not ready
{
    if (!U0_TRANSMIT_HOLDING_REGISTER_EMPTY) return -1; //set immediately upon detection of an empty THR and is cleared on a THR write.
    U0_TRANSMIT_HOLDING_REGISTER = c;                   //newest character in the TX FIFO 
    return 0;
}

static void setBaud(int baud)
{
    int dl = (6000000 + (baud >> 1)) / baud; // 6,000,000 <== 96,000,000 / 16 ; adding 'baud >> 1' rounds the result to the nearest integer rather than rounding down.
    U0_DIVISOR_ACCESS_BIT = 1;
        U0_DIVISOR_MSB = dl >> 8;
        U0_DIVISOR_LSB = dl & 0xFF;
        U0_FRACTIONAL_DIVIDER_REGISTER = DIV_VAL | MUL_VAL << 4;
    U0_DIVISOR_ACCESS_BIT = 0;
}
void SerialPcInit(int baud)
{
    PCONP    |= 1U <<  3; //UART0
    PCLKSEL0 |= 1U <<  6; //UART0
    PINSEL0  |= 1U <<  4; //P0.02 01 TXD0  UART0
    PINSEL0  |= 1U <<  6; //P0.03 01 RXD0  UART0
	
    U0_FIFO_ENABLE = 1;
    
    setBaud(baud);

    U0_LINE_CONTROL_REGISTER |= 0x03; // 8 bit, 1 stop, no parity
}