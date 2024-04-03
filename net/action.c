#include <stdbool.h>

int  ActionMakeFromDestAndTrace(int dest, bool trace)
{
    if (trace) return dest | 0x8000;
    else       return dest & 0x7FFF;
}
int  ActionGetDestPart (int action) { return action & 0x7FFF; }
bool ActionGetTracePart(int action) { return action & 0x8000; }