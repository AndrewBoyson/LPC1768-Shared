#include <stdbool.h>

extern void MacClear(char* mac);
extern void MacCopy(char* macTo, const char* macFrom);
extern bool MacIsEmpty(const char* mac);
extern bool MacIsSame(const char* macA, const char* macB);
extern void MacParse(const char *pText, char *mac);
extern  int MacToString(const char* mac, int size, char* text);
extern  int MacLog(const char* mac);
extern  int MacHttp(const char* mac);
extern char MacLocal[];
extern  int MacAccept(const char* p);
extern void MacMakeFromDest(int dest, int pro, char* p);
