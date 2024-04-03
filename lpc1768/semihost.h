#pragma once

typedef int FILEHANDLE;

typedef struct
{
    unsigned char  hr;   /* Hours    [0..23]                  */
    unsigned char  min;  /* Minutes  [0..59]                  */
    unsigned char  sec;  /* Seconds  [0..59]                  */
    unsigned char  day;  /* Day      [1..31]                  */
    unsigned char  mon;  /* Month    [1..12]                  */
    unsigned short year; /* Year     [1980..2107]             */
} FTIME;

typedef struct
{                        /* File Search info record           */
    char  name[32];      /* File name                         */
    long  size;          /* File size in bytes                */
    int   fileID;        /* System File Identification        */
    FTIME create_time;   /* Date & time file was created      */
    FTIME write_time;    /* Date & time of last write         */
} XFINFO;

#define OPEN_MODE_R    0
#define OPEN_MODE_B    1
#define OPEN_MODE_PLUS 2
#define OPEN_MODE_W    4
#define OPEN_MODE_A    8

extern int        SemihostXffind(const char *pattern, XFINFO *info);
extern FILEHANDLE SemihostOpen  (const char* name, int openmode);
extern int        SemihostClose (FILEHANDLE fh);
extern int        SemihostWrite (FILEHANDLE fh, const char* buffer, int length);
extern int        SemihostRead  (FILEHANDLE fh,       char* buffer, int length);
extern int        SemihostSeek  (FILEHANDLE fh, int position);
extern int        SemihostFlen  (FILEHANDLE fh);

extern int SemihostRemove(const char *name);
extern int SemihostRename(const char *old_name, const char *new_name);

extern void SemihostReset(void);
extern void SemihostMac(char* mac);