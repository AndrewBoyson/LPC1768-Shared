OBJECTS += startup.o
OBJECTS += system.o
OBJECTS += syscalls.o
OBJECTS += led.o
OBJECTS += serialpc.o
OBJECTS += semihost.o
OBJECTS += debug.o
OBJECTS += interrupts.o
OBJECTS += hrtimer/hrtimer.o
OBJECTS += tm/tm.o
OBJECTS += rtc/rtc.o

LIBRARY = lpc1768.a

OPTIMIZATION = 2

ASFLAGS += -mcpu=cortex-m3

GCFLAGS += -mcpu=cortex-m3         #This specifies the name of the target ARM processor
GCFLAGS += -mthumb                 #Generates code that executes in the Thumb state; -marm is the default and generates code that executes in the ARM state.
GCFLAGS += -Wall                   #This enables all the warnings about constructions that some users consider questionable
GCFLAGS += -fno-common             #Specifies that the compiler places uninitialized global variables in the BSS section of the object file; it is the default.
GCFLAGS += -I./                    #adds include directory of header files.
GCFLAGS += -O$(OPTIMIZATION)       #Optimisation level: 0 none; s size; g debugging; 1 some; 2 more; 3 even more

GCC     = arm-none-eabi-gcc
AS      = arm-none-eabi-as
LD      = arm-none-eabi-ld
AR      = arm-none-eabi-ar
OBJCOPY = arm-none-eabi-objcopy
SIZE    = arm-none-eabi-size
DIS     = arm-none-eabi-objdump -dS
MAP     = arm-none-eabi-objdump -h
REMOVE  = rm -f
COPY    = cp -f

all: $(LIBRARY)

$(LIBRARY): $(OBJECTS)
	ar rcs $(LIBRARY) $(OBJECTS)

clean:
	$(REMOVE) $(OBJECTS)
	$(REMOVE) $(LIBRARY)

%.o: %.c
	$(GCC) $(GCFLAGS) -c -o $@ $<

%.o: %.s
	$(AS) $(ASFLAGS) -o $@ $<

.PHONY: all clean