#include <stdio.h>

char* BuildGetDate()
{
	extern char* BuildDate; //Symbol added during linking using --defsym in the makefile
	
	static char text[20]; //Format yyyymmdd
	
	snprintf(text, sizeof(text)-1, "%08u", (unsigned)&BuildDate);
	
	return text;
}