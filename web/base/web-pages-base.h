#include <stdint.h>
#include <stdbool.h>

extern void        WebLoginHtml     (void);
extern void        WebLoginQuery    (char* pQuery);
extern bool        WebLoginQueryPasswordOk;
extern int         WebLoginOriginalToDo;
extern bool        WebLoginCookiesContainValidSessionId(char* pCookies);
extern char*       WebLoginSessionNameGet(void);
extern int         WebLoginSessionNameLife(void);
extern char*       WebLoginSessionIdGet(void);
extern void        WebLoginSessionIdNew(void);
extern bool        WebLoginSessionIdIsSet(void);
extern void        WebLoginForceNewPassword (void);
extern void        WebLoginInit(void);

extern void        WebFavicon       (void);
extern const char* WebFaviconDate;
extern const char* WebFaviconTime;
extern const int   WebFaviconSize;

extern void        WebBaseCss       (void);
extern const char* WebBaseCssDate;
extern const char* WebBaseCssTime;
extern void        WebNavCss        (void);
extern const char* WebNavCssDate;
extern const char* WebNavCssTime;

extern void        WebClockHtml     (void);
extern void        WebClockScript   (void);
extern const char* WebClockScriptDate;
extern const char* WebClockScriptTime;
extern void        WebClockAjax     (void);
extern void        WebClockQuery    (char* pQuery);

extern void        WebLogHtml       (void);
extern void        WebLogQuery      (char* pQuery);

extern void        WebTraceHtml     (void);
extern void        WebTraceScript   (void);
extern const char* WebTraceScriptDate;
extern const char* WebTraceScriptTime;
extern void        WebTraceAjax     (void);
extern void        WebTraceQuery    (char* pQuery);

extern void        WebNetHtml       (void);
extern void        WebNetScript     (void);
extern const char* WebNetScriptDate;
extern const char* WebNetScriptTime;
extern void        WebNetAjax       (void);
extern void        WebNetQuery      (char* pQuery);

extern void        WebNet4Html      (void);
extern void        WebNet4Script    (void);
extern const char* WebNet4ScriptDate;
extern const char* WebNet4ScriptTime;
extern void        WebNet4Ajax      (void);

extern void        WebNet6Html      (void);
extern void        WebNet6Script    (void);
extern const char* WebNet6ScriptDate;
extern const char* WebNet6ScriptTime;
extern void        WebNet6Ajax      (void);

extern void        WebResetHtml     (void);
extern void        WebResetQuery    (char* pQuery);

extern void        WebFirmwareHtml  (void);
extern void        WebFirmwareScript(void);
extern const char* WebFirmwareScriptDate;
extern const char* WebFirmwareScriptTime;
extern void        WebFirmwareQuery (char* pQuery);
extern int         WebFirmwareTargetLength;
extern int         WebFirmwareActualLength;
extern char*       WebFirmwareFileName;
extern void        WebFirmwarePost  (int contentLength, int contentStart, int size, char* pRequestStream, uint32_t positionInRequestStream, bool* pComplete);
extern void        WebFirmwareAjax  (void);
