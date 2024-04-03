extern char SlaacLinkLocalIp[];
extern char SlaacUniqueLocalIp[];
extern char SlaacGlobalIp[];

extern int  SlaacScope(char* ip);
extern void SlaacAddressFromScope(int scope, char* pSrcIp);

extern void SlaacMakeGlobal     (char* pPrefix);
extern void SlaacMakeUniqueLocal(char* pPrefix);

extern void SlaacInit(void);