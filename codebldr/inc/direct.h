/* direct.h - directory manipulation function prototypes
 * $Version: 1.9 $
 * Copyright 1988,89,90,91 Intel Corporation, ALL RIGHTS RESERVED.
 */

#ifndef _directh
#define _directh

/*
 * Function prototypes:
 */
int    chdir(const char *);
int   _chdrive(int);
char  *getcwd(char *, int);
char *_getdcwd(int, char *, int);
int   _getdrive(void);
int    mkdir(const char *);
int    rmdir(const char *);

#endif /* _directh */
