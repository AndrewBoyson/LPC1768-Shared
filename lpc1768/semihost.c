#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "semihost.h"

#define SYS_OPEN   0x1
#define SYS_CLOSE  0x2
#define SYS_WRITE  0x5
#define SYS_READ   0x6
#define SYS_SEEK   0xa
#define SYS_FLEN   0xc
#define SYS_REMOVE 0xe
#define SYS_RENAME 0xf
#define USR_XFFIND 0x100
#define USR_UID    0x101
#define USR_RESET  0x102

#define DEVICE_ID_LENGTH  32
#define DEVICE_MAC_OFFSET 20

//Be careful - semihost calls block interrupts so only use during initialisation or resets.
static inline int __attribute__ ((always_inline)) __semihost(int reason, void *arg) {
  int value;
  __asm volatile (
      "mov r0, %[rsn] \n"
      "mov r1, %[arg] \n"
      "bkpt 0xAB      \n"
      "mov %[val], r0 \n"

      : [val] "=r" (value) /* outputs */
      : [rsn] "r" (reason), [arg] "r" (arg) /* inputs */
      : "r0", "r1", "r2", "r3", "ip", "lr", "memory", "cc" /* clobber */
  );
  return value;
}
int SemihostXffind (const char *pattern, XFINFO *info)
{
    uint32_t args[4];

    args[0] = (unsigned long)pattern;
    args[1] = (unsigned long)strlen(pattern);
    args[2] = (unsigned long)info;
    args[3] = (unsigned long)sizeof(XFINFO);

    return __semihost(USR_XFFIND, args);
}

FILEHANDLE SemihostOpen(const char* name, int openmode)
{
//On exit, R0 contains: a nonzero handle if the call is successful; –1 if the call is not successful.
    uint32_t args[3];
    args[0] = (uint32_t)name;
    args[1] = (uint32_t)openmode;
    args[2] = (uint32_t)strlen(name);
    return __semihost(SYS_OPEN, args);
}

int SemihostClose(FILEHANDLE fh)
{
//On exit, R0 contains: 0 if the call is successful; –1 if the call is not successful.
    return __semihost(SYS_CLOSE, &fh);
}

int SemihostWrite(FILEHANDLE fh, const char* buffer, int length) //returns the number of bytes written or < length if there is an error
{
//On exit, R0 contains: 0 if the call is successful; the number of bytes that are not written, if there is an error.
    if (length == 0) return 0;

    uint32_t args[3];
    args[0] = (uint32_t)fh;
    args[1] = (uint32_t)buffer;
    args[2] = (uint32_t)length;
    return length - __semihost(SYS_WRITE, args);
}

int SemihostRead(FILEHANDLE fh, char* buffer, int length) //Returns the number of bytes read or 0 if EOF or error
{
/*
    R0 contains zero if the call is successful.
    If R0 contains the same value as word 3, the call has failed and EOF is assumed.
    If R0 contains a smaller value than word 3, the call was partially successful. No error is assumed, but the buffer has not been filled.
*/
    uint32_t args[3];
    args[0] = (uint32_t)fh;
    args[1] = (uint32_t)buffer;
    args[2] = (uint32_t)length;
    return length - __semihost(SYS_READ, args);
}
int SemihostSeek(FILEHANDLE fh, int position)
{
//On exit, R0 contains: 0 if the request is successful; A negative value if the request is not successful.

    uint32_t args[2];
    args[0] = (uint32_t)fh;
    args[1] = (uint32_t)position;
    return __semihost(SYS_SEEK, args);
}
int SemihostFlen(FILEHANDLE fh)
{
//On exit, R0 contains: the current length of the file object, if the call is successful; –1 if an error occurs.
    return __semihost(SYS_FLEN, &fh);
}
int SemihostRemove(const char *name)
{
//On exit, R0 contains: 0 if the delete is successful; a nonzero, host-specific error code if the delete fails.

    uint32_t args[2];
    args[0] = (uint32_t)name;
    args[1] = (uint32_t)strlen(name);
    return __semihost(SYS_REMOVE, args);
}

int SemihostRename(const char *old_name, const char *new_name)
{
//On exit, R0 contains: 0 if the rename is successful; a nonzero, host-specific error code if the rename fails.

    uint32_t args[4];
    args[0] = (uint32_t)old_name;
    args[1] = (uint32_t)strlen(old_name);
    args[2] = (uint32_t)new_name;
    args[3] = (uint32_t)strlen(new_name);
    return __semihost(SYS_RENAME, args);
}

void SemihostMac(char *mac)
{
    char uid[DEVICE_ID_LENGTH + 1];
    uint32_t args[2];
    args[0] = (uint32_t)uid;
    args[1] = DEVICE_ID_LENGTH + 1;

    __semihost(USR_UID, &args);
    
    char *p = uid;
    p += DEVICE_MAC_OFFSET;
    for (int i=0; i<6; i++)
    {
        int byte;
        sscanf(p, "%2x", &byte);
        mac[i] = byte;
        p += 2;
    }
    mac[0] &= ~0x01;    // reset the IG bit in the address; see IEE 802.3-2002, Section 3.2.3(b)
}
void SemihostReset()
{
    __semihost(USR_RESET, NULL);
}