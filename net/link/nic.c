#include <stdint.h>

#include "nicdefs.h"
#include "lpc1768/semihost.h"
#include "lpc1768/led.h"
#include "lpc1768/debug.h"
#include "log/log.h"

#define NUM_RX_FRAMES       6           // Number of Rx Frames (== packets) was 3
#define NUM_TX_FRAMES       4           // Number of Tx Frames (== packets) was 2

#define ETH_FRAME_LEN       1536        // Maximum Ethernet Frame Size
/*
Total length is NUM_RX * ((2 * 4) + (2 * 4) + 0x600) + NUM_TX * ((2 * 4) + (1 * 4) + 0x600)
                    1  *  1552                              1548    
                    
Can fit up to 10 in total
eg 6 * 1552 + 4 * 1548 = 9312 + 6192 = 15504
*/


__attribute__((section(".ethram"),aligned(4))) static volatile         uint8_t r_buff[NUM_RX_FRAMES][ETH_FRAME_LEN];
__attribute__((section(".ethram"),aligned(4))) static volatile         uint8_t t_buff[NUM_TX_FRAMES][ETH_FRAME_LEN];
__attribute__((section(".ethram"),aligned(4))) static volatile RX_DESC_TypeDef r_desc[NUM_RX_FRAMES];
__attribute__((section(".ethram"),aligned(8))) static volatile RX_STAT_TypeDef r_stat[NUM_RX_FRAMES]; //Must be aligned on an 8 byte boundary
__attribute__((section(".ethram"),aligned(4))) static volatile TX_DESC_TypeDef t_desc[NUM_TX_FRAMES];
__attribute__((section(".ethram"),aligned(4))) static volatile TX_STAT_TypeDef t_stat[NUM_TX_FRAMES];

char* NicGetReceivedPacketOrNull(int* pSize)
{   
    if (RX_PRODUCE_INDEX == RX_CONSUME_INDEX) return NULL;
        
    uint32_t info = r_stat[RX_CONSUME_INDEX].Info;
    *pSize = (info & RINFO_SIZE) + 1 - 4; // exclude checksum
	
    return (char*)r_buff[RX_CONSUME_INDEX];
}
void NicReleaseReceivedPacket()
{
    if (RX_CONSUME_INDEX == RX_DESCRIPTOR_NUMBER) RX_CONSUME_INDEX = 0;
    else                                          RX_CONSUME_INDEX++;
}
char* NicGetTransmitPacketOrNull(int* pSize)
{
    if (TX_CONSUME_INDEX == 0 && TX_PRODUCE_INDEX == TX_DESCRIPTOR_NUMBER) return NULL;
    if (TX_PRODUCE_INDEX == TX_CONSUME_INDEX - 1)                          return NULL;
    *pSize = ETH_FRAME_LEN - 4;
    return (char*)t_buff[TX_PRODUCE_INDEX];
}
void NicSendTransmitPacket(int size)
{
    if (size == 0) return;
    t_desc[TX_PRODUCE_INDEX].Ctrl = (size - 1) | (TCTRL_INT | TCTRL_LAST);
    if (TX_PRODUCE_INDEX == TX_DESCRIPTOR_NUMBER) TX_PRODUCE_INDEX = 0;
    else                                          TX_PRODUCE_INDEX++;
}

static void txdscr_init()
{
    int i;

    for(i = 0; i < NUM_TX_FRAMES; i++)
    {
        t_desc[i].Packet = (uint32_t)&t_buff[i];
        t_desc[i].Ctrl   = 0;
        t_stat[i].Info   = 0;
    }

    TX_DESCRIPTOR       = (uint32_t)t_desc;         /* Set EMAC Transmit Descriptor Registers. */
    TX_STATUS           = (uint32_t)t_stat;
    TX_DESCRIPTOR_NUMBER = NUM_TX_FRAMES - 1;

    TX_PRODUCE_INDEX  = 0;                           /* Tx Descriptors Point to 0 */
}

static void rxdscr_init()
{
    int i;

    for(i = 0; i < NUM_RX_FRAMES; i++)
    {
        r_desc[i].Packet  = (uint32_t)&r_buff[i];
        r_desc[i].Ctrl    = RCTRL_INT | (ETH_FRAME_LEN-1);
        r_stat[i].Info    = 0;
        r_stat[i].HashCRC = 0;
    }

    RX_DESCRIPTOR       = (uint32_t)r_desc;        /* Set EMAC Receive Descriptor Registers. */
    RX_STATUS           = (uint32_t)r_stat;        //Must be aligned on an 8 byte boundary
    RX_DESCRIPTOR_NUMBER = NUM_RX_FRAMES - 1;

    RX_CONSUME_INDEX  = 0;                          /* Rx Descriptors Point to 0 */
}
static int phy_write(unsigned int PhyReg, unsigned short Data)
{
    unsigned int timeOut;

    MADR = DP83848C_DEF_ADR | PhyReg;
    MWTD = Data;

    // Wait until operation completed
    for(timeOut = 0; timeOut < MII_WR_TOUT; timeOut++)
    {
        if((MIND & MIND_BUSY) == 0)  return 0;
    }

    //Timed out
    return -1;
}

static int phy_read(unsigned int PhyReg)
{
    unsigned int timeOut;

    MADR = DP83848C_DEF_ADR | PhyReg;
    MCMD = MCMD_READ;

    // Wait until operation completed
    for(timeOut = 0; timeOut < MII_RD_TOUT; timeOut++)
    {
        if((MIND & MIND_BUSY) == 0)
        {
            MCMD = 0;
            return MRDD; // Return a 16-bit value.
        }
    }
    return -1;
}

void NicLinkAddress(char *mac)
{
    mac[5] = SA0 >> 8;
    mac[4] = SA0 & 0xFF;
    mac[3] = SA1 >> 8;
    mac[2] = SA1 & 0xFF;
    mac[1] = SA2 >> 8;
    mac[0] = SA2 & 0xFF;
}

void NicLinkSetSpeedDuplex(int speed, int duplex)
{
    unsigned short phy_data;
    volatile int tout; //This will hopefully prevent the short delays from being optimised out

    if((speed < 0) || (speed > 1)) phy_data = PHY_AUTO_NEG;
    else                           phy_data = (((unsigned short) speed << 13) | ((unsigned short) duplex << 8));

    phy_write(PHY_REG_BMCR, phy_data);

    for(tout = 100; tout; tout--) asm("NOP");     /* A short delay */
    
    phy_data = phy_read(PHY_REG_STS);

    if(phy_data & PHY_STS_DUPLEX)
    {
        MAC2    |= MAC2_FULL_DUP;
        COMMAND |=   CR_FULL_DUP;
        IPGT     = IPGT_FULL_DUP;
    }
    else
    {
        MAC2    &= ~MAC2_FULL_DUP;
        COMMAND &=   ~CR_FULL_DUP;
        IPGT     =  IPGT_HALF_DUP;
    }

    if(phy_data & PHY_STS_SPEED)
    {
        SUPP &= ~SUPP_SPEED;
    }
    else
    {
        SUPP |= SUPP_SPEED;
    }
}

int NicLinkIsUp(void)
{
    return (phy_read(PHY_REG_STS) & PHY_STS_LINK);
}
static int phy_reset()
{
    int regv, tout;
    
    // perform PHY reset
    phy_write(PHY_REG_BMCR, PHY_BMCR_RESET);           
  
    // Wait for hardware reset to end.
    for(tout = 0x20000; ; tout--)
    {                    
        regv = phy_read(PHY_REG_BMCR);
        if(regv < 0 || tout == 0)     return -1;  // Error
        if(!(regv & PHY_BMCR_RESET))  break;      // Reset complete.
    }
    uint32_t phy_id  = (phy_read(PHY_REG_IDR1) << 16);
    phy_id |= (phy_read(PHY_REG_IDR2) & 0XFFF0);

    //Check is the right PHY
    if (phy_id != DP83848C_ID)
    {
        LogTimeF("Unknown Ethernet PHY (%x)", (unsigned int)phy_id);
        return -1;
    }
    return 0;
}
int NicInit()
{
    volatile int tout; //This will hopefully prevent the short delays from being optimised out
    char mac[6];
    unsigned int clock = 10; //96,000,000
    
    // Reset all EMAC internal modules.
    MAC1    = MAC1_RES_TX | MAC1_RES_MCS_TX | MAC1_RES_RX | MAC1_RES_MCS_RX | MAC1_SIM_RES | MAC1_SOFT_RES;
    COMMAND = CR_REG_RES | CR_TX_RES | CR_RX_RES | CR_PASS_RUNT_FRM;
    
    // A short delay after reset.
    for(tout = 100; tout; tout--) asm("NOP");             
    
    // Initialize MAC control registers.
    MAC1 = MAC1_PASS_ALL;                   
    MAC2 = MAC2_CRC_EN | MAC2_PAD_EN;
    MAXF = ETH_FRAME_LEN;
    CLRT = CLRT_DEF;
    IPGR = IPGR_DEF;
    
    // Enable Reduced MII interface.
    COMMAND = CR_RMII | CR_PASS_RUNT_FRM;    
    
    // Set clock and reset
    MCFG = (clock << 0x2) & MCFG_CLK_SEL;
    MCFG |= MCFG_RES_MII;
    
    // A short delay after reset
    for(tout = 100; tout; tout--) asm("NOP");  
    
    // Set clock
    MCFG = (clock << 0x2) & MCFG_CLK_SEL;
    MCMD = 0;
    
    // Reset Reduced MII Logic.
    SUPP = SUPP_RES_RMII;                    
    
    // A short delay
    for (tout = 100; tout; tout--) asm("NOP");  
    
    SUPP = 0;
    
    //Reset the PHY
    if (phy_reset()) return -1;
    
    //Set the link to auto negotiate
    NicLinkSetSpeedDuplex(-1, 0);
    
    // Set the Ethernet MAC Address registers
    SemihostMac(mac);
	DebugWriteText("MAC ");
	DebugWriteHex(mac[0]);
	DebugWriteHex(mac[1]);
	DebugWriteHex(mac[2]);
	DebugWriteHex(mac[3]);
	DebugWriteHex(mac[4]);
	DebugWriteHex(mac[5]);
	DebugWriteText("\r");
    SA0 = ((uint32_t)mac[5] << 8) | (uint32_t)mac[4];
    SA1 = ((uint32_t)mac[3] << 8) | (uint32_t)mac[2];
    SA2 = ((uint32_t)mac[1] << 8) | (uint32_t)mac[0];
    
    //Initialise DMA descriptors
    txdscr_init();
    rxdscr_init();
    
    // Set filter
    RX_FILTER_CTRL = RFC_UCAST_EN | RFC_MCAST_EN | RFC_BCAST_EN | RFC_PERFECT_EN;
                                                      
    // Disable and clear EMAC interrupts
    INT_ENABLE = 0;
    INT_CLEAR  = 0xFFFF;
    
    //Enable receive and transmit
    COMMAND  |= (CR_RX_EN | CR_TX_EN);
    MAC1     |= MAC1_REC_EN;
    
    //Return success
    return 0;
}
