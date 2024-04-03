extern void SpiInit(void);
extern void SpiChipSelect(int value);
extern void SpiWrite(char byte);
extern int  SpiBusy(void);
extern char SpiRead(void);
extern char SpiTransfer(char byte);