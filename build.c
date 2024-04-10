extern char* BuildGetDate()
{
	//__DATE__ --> mmm dd yyyy (e.g. "Jan 14 2012")
	static char dd_mmm_yyyy[12];
	dd_mmm_yyyy[ 0] = __DATE__[4];
	dd_mmm_yyyy[ 1] = __DATE__[5];
	dd_mmm_yyyy[ 2] = ' ';
	dd_mmm_yyyy[ 3] = __DATE__[0];
	dd_mmm_yyyy[ 4] = __DATE__[1];
	dd_mmm_yyyy[ 5] = __DATE__[2];
	dd_mmm_yyyy[ 6] = ' ';
	dd_mmm_yyyy[ 7] = __DATE__[7];
	dd_mmm_yyyy[ 8] = __DATE__[8];
	dd_mmm_yyyy[ 9] = __DATE__[9];
	dd_mmm_yyyy[10] = __DATE__[10];
	dd_mmm_yyyy[11] = 0;
	return dd_mmm_yyyy;
}