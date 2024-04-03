#include <stdint.h>
#include <stdbool.h>

extern int  WebServerBaseDecideWhatToDo(char *pPath, char* pLastModified);
extern bool WebServerBaseHandleQuery   (int todo, char* pQuery);
extern bool WebServerBasePost          (int todo, int contentLength, int contentStart, int size, char* pRequestStream, uint32_t positionInRequestStream, bool* pComplete);
extern bool WebServerBaseReply         (int todo);
