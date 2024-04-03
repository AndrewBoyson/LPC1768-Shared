#include <stdbool.h>

extern void  WebLoginSessionNameCreate(void);
extern void  WebLoginSessionIdReset(void);

extern void  WebLoginPasswordRestore  (void);
extern void  WebLoginPasswordReset    (void);
extern void  WebLoginPasswordSet      (char* password);
extern bool  WebLoginPasswordIsSet    (void);
extern bool  WebLoginPasswordMatches  (char* password);