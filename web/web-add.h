#include <stdint.h>
#include <stdbool.h>

extern void WebAddNavItem      (int highlight, const char* href, const char* title);
extern void WebAddNav          (int page);
extern void WebAddHeader       (const char* title, const char* style, const char* script);
extern void WebAddH1           (const char* pageName);
extern void WebAddH2           (const char* text);
extern void WebAddEnd          (void);

extern void WebAddLabelledText        (const char* label,                       const char* text);
extern void WebAddLabelledPrefixSuffix(const char* label,   const char* prefix, const char* text,  const char* suffix);
extern void WebAddLabelledMac         (const char* label,                       const char*   mac);
extern void WebAddLabelledIp4         (const char* label,                       uint32_t       ip);
extern void WebAddLabelledIp6         (const char* label,                       const char*    ip);
extern void WebAddLabelledOnOff       (const char* label,                       bool        value);
extern void WebAddLabelledLed         (const char* label,                       bool        value);
extern void WebAddLabelledInt         (const char* label,                       int         value);

extern void WebAddInputText           (const char* label,   float inputwidth,   const char* value, const char* action, const char* name);
extern void WebAddInputInt            (const char* label,   float inputwidth,   int         value, const char* action, const char* name);
extern void WebAddInputButton         (const char* label,                       const char* value, const char* action, const char* name);

extern void WebAddAjaxLed             (const char* label,                       const char* id);
extern void WebAddAjaxButton          (const char* label,                                       const char* name);
extern void WebAddAjaxLabelled        (const char* label,                       const char* id);
extern void WebAddAjaxLabelledSuffix  (const char* label,                       const char* id,                   const char* suffix);
extern void WebAddAjaxInputToggle     (const char* label,                       const char* id, const char* name);
extern void WebAddAjaxInput           (const char* label,   float inputwidth,   const char* id, const char* name);
extern void WebAddAjaxInputSuffix     (const char* label,   float inputwidth,   const char* id, const char* name, const char* suffix);
extern void WebAddAjaxInputLabelId    (const char* labelId, float inputwidth,   const char* id, const char* name);
