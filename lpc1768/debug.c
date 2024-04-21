#include <stdlib.h>
#include "serialpc.h"

void DebugWriteChar(char c)
{
	SerialPcInit(115200);
	while (1) { int r = SerialPcPutC(c); if (!r) break; }
}
void DebugWriteText(char* text)
{
	SerialPcInit(115200);
	for (char* p = text; *p; p++)
	{
		while (1) { int r = SerialPcPutC(*p); if (!r) break; }
	}
}

void DebugWriteDecimal(int number)
{
	char text[20];
	itoa(number, text, 10);
	DebugWriteText(text);
}

void DebugWriteHex(int number)
{
	char text[20];
	itoa(number, text, 16);
	DebugWriteText(text);
}