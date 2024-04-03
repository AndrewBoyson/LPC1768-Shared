#include <stdint.h>
#include <stdbool.h>

#define DO_NOTHING           0
#define DO_NOT_FOUND         1
#define DO_NOT_MODIFIED      2
#define DO_SERVER           10
#define DO_BASE            100
#define DO_THIS            200
#define DO_SEND_SESSION_ID 300

extern bool WebTrace;

extern void WebInit    (void);

extern void WebAddResponse   (int todo);
extern  int WebHandleQuery   (char* pQuery, char* pCookies, int* pTodo, uint32_t* pDelayUntil); //return -1 on stop; 0 on continue
extern void WebHandlePost    (int todo, int contentLength, int contentStart, int size, char* pRequestStream, uint32_t positionInRequestStream, bool* pComplete);
extern int  WebDecideWhatToDo(char *pPath, char* pLastModified);
