
void Startup()
{
	extern long _sbss, _ebss, _sdata, _edata, _sidata;
	for (long *dst = &_sbss; dst < &_ebss; dst++) *dst = 0;
	for (long *dst = &_sdata, *src = &_sidata; dst < &_edata;) *dst++ = *src++;
}