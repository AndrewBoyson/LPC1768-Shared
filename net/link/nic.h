extern int  NicInit(void);

extern void NicLinkAddress(char *mac);                 // get the ethernet address
extern int  NicLinkIsUp(void);                         // see if the link is up
extern void NicLinkSpeedDuplex(int speed, int duplex); // force link settings

extern char* NicGetReceivedPacketOrNull(int* pSize);
extern void  NicReleaseReceivedPacket(void);
extern char* NicGetTransmitPacketOrNull(int* pSize);
extern void  NicSendTransmitPacket(int size);
