OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(main)

PROVIDE(OneWireBusHandler = DefaultHandler); /*If OneWireBusHandler is not defined (in vectors) then substitute the DefaultHandler*/

MEMORY
{
	flash  (rx) : ORIGIN = 0x00000000, LENGTH = 512K
	sram   (rw) : ORIGIN = 0x10000000, LENGTH =  32K
	usbram (rw) : ORIGIN = 0x2007c000, LENGTH =  16K
	ethram (rw) : ORIGIN = 0x20080000, LENGTH =  16K
}

_stack = ORIGIN(sram) + LENGTH(sram); /* Define the top of our stack at the end of SRAM */

SECTIONS
{
	.usbram (NOLOAD) :
	{
	} >usbram
	
	.ethram (NOLOAD) :
	{
		*(.ethram)
		*(.persist)
	} >ethram
	
	.text :
	{
		*(.vectors)
		*(.startup)
		*(.text)
		*(.text.*)
		*(.rodata)
		*(.rodata.*)
		*(.glue_7)
		*(.glue_7t)
		*(.ARM*)
	} >flash

	.data :
	{
		_sdata = .;
		*(.data)
		*(.data.*)
	} >sram AT >flash
	_sidata = LOADADDR(.data);
	. = ALIGN(4);
	_edata = .;
	
	.bss (NOLOAD) :
	{
		_sbss = .;
		*(.bss)
		*(.bss.*)
	} >sram
	. = ALIGN(4);
	_ebss = .;
	_heap = .;	/* Define our heap after data and bss sections in SRAM */
	
	/DISCARD/ :
	{
		*(.debug*)
	}
}
