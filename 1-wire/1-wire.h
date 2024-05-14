#include <stdbool.h>

#define ONE_WIRE_RESULT_OK                      0
#define ONE_WIRE_RESULT_NO_DEVICE_PRESENT       1
#define ONE_WIRE_RESULT_NO_DEVICE_PARTICIPATING 2
#define ONE_WIRE_RESULT_TIMED_OUT               3
#define ONE_WIRE_RESULT_CRC_ERROR               4

extern char OneWireInUse;
extern char OneWireTrace;

extern int  OneWireResult(void);
extern void OneWireInit(char* pin);
extern bool OneWireBusy(void);
extern void OneWireExchange(int lenBytesToSend, int lenBytesToRecv, char *pBytesToSend, char *pBytesToRecv, int msToPullUp);
extern void OneWireSearch(char command, char* pDeviceRomList, bool* pAllDevicesFound);
extern void OneWireMain(void);
