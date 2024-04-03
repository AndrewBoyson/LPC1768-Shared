extern int  SerialPcGetC(void);   // Returns a negative number if no character to read or if there was an error. 0 to 255 otherwise.
extern int  SerialPcPutC(char c); // Returns zero on success or -1 if the buffer was full or not ready
extern void SerialPcInit(int baud);
