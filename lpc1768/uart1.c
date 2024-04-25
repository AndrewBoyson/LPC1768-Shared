#include "lpc1768/register.h"
#include "lpc1768/bitband.h"

#define DIV_VAL     0 //0 == do not use
#define MUL_VAL     1 //Cannot be zero

int Uart1GetC() // Returns a negative number if no character to read or if there was an error. 0 to 255 otherwise.
{
    if (!U1_RECEIVER_DATA_READY) return -1;
    return U1_RECEIVE_BUFFER_REGISTER;      //oldest character in the RX FIFO
}
int Uart1PutC(char c) // Returns zero on success or -1 if the buffer was full or not ready
{
    if (!U1_TRANSMIT_HOLDING_REGISTER_EMPTY) return -1; //set immediately upon detection of an empty THR and is cleared on a THR write.
    U1_TRANSMIT_HOLDING_REGISTER = c;                   //newest character in the TX FIFO 
    return 0;
}

void Uart1SetBaud(int baud)
{
    int dl = (6000000 + (baud >> 1)) / baud; // 6,000,000 <== 96,000,000 / 16 ; adding 'baud >> 1' rounds the result to the nearest integer rather than rounding down.
    U1_DIVISOR_ACCESS_BIT = 1;
        U1_DIVISOR_MSB = dl >> 8;
        U1_DIVISOR_LSB = dl & 0xFF;
        U1_FRACTIONAL_DIVIDER_REGISTER = DIV_VAL | MUL_VAL << 4;
    U1_DIVISOR_ACCESS_BIT = 0;
}
void Uart1Init(int baud)
{            
    PCONP    |= 1U <<  4; //UART1
    PCLKSEL0 |= 1U <<  8; //UART1
	PINSEL0  |= 1U << 30; //P0.15 01 TXD1  UART1
    PINSEL1  |= 1U <<  0; //P0.16 01 RXD1  UART1
	
    U1_FIFO_ENABLE = 1;
    
    Uart1SetBaud(baud);

    U1_LINE_CONTROL_REGISTER |= 0x03; // 8 bit, 1 stop, no parity
}
