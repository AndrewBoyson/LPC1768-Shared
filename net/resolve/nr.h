#include <stdint.h>
#include <stdbool.h>

extern bool Nr4Trace; //Spare
extern bool NrTrace;

#define NR_NAME_MAX_LENGTH     40

extern void NrMakeRequestForNameFromAddress6(char*    address6);
extern void NrMakeRequestForNameFromAddress4(uint32_t address4);
extern void NrMakeRequestForAddress6FromName(char*    name);
extern void NrMakeRequestForAddress4FromName(char*    name);

extern void NrAddAddress6(char*    address, char* name, int dnsProtocol);
extern void NrAddAddress4(uint32_t address, char* name, int dnsProtocol);

extern void NrAddress6ToName(char*    address6, char*       name);
extern void NrAddress4ToName(uint32_t address4, char*       name);
extern void NrNameToAddress6(char*    name,     char*       address6);
extern void NrNameToAddress4(char*    name,     uint32_t*  pAddress4);

extern void NrSendAjax(void);

extern void NrMain(void);
extern void NrInit(void);
