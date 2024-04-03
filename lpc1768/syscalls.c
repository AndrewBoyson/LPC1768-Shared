#include <sys/stat.h>
#include <stdlib.h>

#include "debug.h"
#include "led.h"

void *_sbrk(int incr)
{
	extern int _heap;
	static unsigned char *heap = NULL;
	unsigned char *prev_heap;

	if (heap == NULL) heap = (unsigned char *)&_heap;
	prev_heap = heap;

	heap += incr;

	DebugWriteText("_sbrk was called with incr ");
	DebugWriteDecimal(incr);
	DebugWriteText(" heap size is now ");
	unsigned char *pStartHeap = (unsigned char *)&_heap;
	unsigned char *pEndHeap   = (unsigned char *)heap;
	DebugWriteDecimal(pEndHeap - pStartHeap);
	DebugWriteText(" bytes\r");

	return prev_heap;
}

int _close(int file) {
  return -1;
}

int _fstat(int file, struct stat *st) {
  st->st_mode = S_IFCHR;

  return 0;
}

int _isatty(int file) {
  return 1;
}

int _lseek(int file, int ptr, int dir) {
  return 0;
}


void _exit(int status) {
  __asm("BKPT #0");
  while(1);
}


void _kill(int pid, int sig) {
  return;
}

int _getpid(void) {
  return -1;
}

int _write (int file, char * ptr, int len) {
  return -1;
}

int _read (int file, char * ptr, int len) {
  return -1;
}

