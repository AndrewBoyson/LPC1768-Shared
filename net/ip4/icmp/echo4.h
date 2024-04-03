extern bool Echo4Trace;

extern int  Echo4HandleRequest(void (*traceback)(void), uint8_t* pType, uint8_t* pCode, char* payloadRx, int sizeRx, char* payloadTx, int* pSizeTx);
