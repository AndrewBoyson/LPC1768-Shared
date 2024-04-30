#include "lpc1768/register.h"
#include "lpc1768/gpio.h"
#include "log/log.h"

#define CS_DIR FIO0DIR_ALIAS(6)
#define CS_SET FIO0SET_BIT(6)
#define CS_CLR FIO0CLR_BIT(6)

void SpiInit(void)
{
	//Power, clock and pins
    PCONP    |= 1U << 10; //SSP1
    PCLKSEL0 |= 1U << 20; //SSP1
    PINSEL0  |= 2U << 14; //P0.07 10 SCK1   SSP1
    PINSEL0  |= 2U << 16; //P0.08 10 MISO1  SSP1
    PINSEL0  |= 2U << 18; //P0.09 10 MOSI1  SSP1
	
    //Configure
    SSP1CR0 |= 7 << 0; //3:0 8 bit transfer
    SSP1CR0 |= 0 << 4; //5:4 SPI
    SSP1CR0 |= 0 << 6; //7:6 Mode 0
    SSP1CR0 |= 0 << 8; //divide by 1

    //Set prescaler bps = PCLK / PS ==> PS = PCLK / bps ==> PS = 96/16 = 6
    SSP1CPSR = 6; //Bit 0 must be 0. 6 ==> 16 bps which is within the 20MHz allowed by the FRAM
    
    //Select the function of the ssel pin: P0.6
    CS_SET;     //Deselect the output == CS = 1
    CS_DIR = 1; //Set the direction to 1 == output
    
    //Enable operation
    SSP1CR1 |= 2; //Enable the SSP controller
}
void SpiChipSelect(int value)
{
    if (value) CS_SET;
    else       CS_CLR;
}
void SpiWrite(char byte)
{
    SSP1DR = byte; //This loads the next frame in the TX FIFO
}
int  SpiBusy(void)
{
    return SSP1SR & 0x10; //bit 4 is BSY. This bit is 0 if the SSPn controller is idle, or 1 if it is currently sending/receiving a frame and/or the Tx FIFO is not empty.
}
char SpiRead(void)
{
    return SSP1DR & 0xFF; //This reads the oldest frame in the RX FIFO
}
char SpiTransfer(char byte)
{
    SpiWrite(byte);
    while(SpiBusy()) /*spin until not busy, at 16 bits per us or 2 bytes per us should be only 48 operations*/;
    return SpiRead();
}
