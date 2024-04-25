
extern int  Uart1GetC(void);   // Returns a negative number if no character to read or if there was an error. 0 to 255 otherwise.
extern int  Uart1PutC(char c); // Returns zero on success or -1 if the buffer was full or not ready
extern void Uart1SetBaud(int baud);
extern void Uart1Init(int baud);