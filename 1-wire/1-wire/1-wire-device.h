#define DEVICE_MAX 8

#define DEVICE_ADDRESS_STRING_LENGTH 24

extern int     DeviceCount;
extern char    DeviceList[];
extern void    DeviceAddressToString(char* pAddress, char* buffer);
extern void    DeviceParseAddress(char* pText, char *pAddress);

extern int  DeviceScanMs;
extern bool DeviceBusy(void);
extern void DeviceInit(void);
extern int  DeviceMain(void);