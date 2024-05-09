
extern void (*GpioHook)(void);
extern void GpioInit(void);
extern void GpioInterruptsEnable();
extern void GpioInterruptsDisable();

extern void GpioParse(char* pin, int* port, int* bit); //Expect something like "p0.12" or "P1-2"
extern unsigned* GpioSetPtr(char* pin);
extern unsigned* GpioClrPtr(char* pin);
extern unsigned* GpioDirPtr(char* pin);
extern unsigned* GpioPinPtr(char* pin);