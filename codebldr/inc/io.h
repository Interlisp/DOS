/* io.h - file access function prototypes
 * $Version: 1.11 $
 * Copyright 1988,89,90,91 Intel Corporation, ALL RIGHTS RESERVED.
 */

#ifndef _ioh
#define _ioh

#ifndef _typesh
#include <sys/types.h>
#endif

/*
 * Function prototypes:
 */
int    access(const char *, int);
int    chmod(const char *, mode_t);
int    chsize(int, long);
int    close(int);
int    creat(const char *, mode_t);
int    dup(int);
int    dup2(int, int);
int    eof(int);
long   filelength(int);
int    isatty(int);
int    locking(int, int, long);
off_t  lseek(int, off_t, int);
long   ltell(int);
int   _map_length(int, const void *, size_t);
char  *mktemp(char *);
int    open(const char *, int, ...);
int   _open(const char *, unsigned int, unsigned int, unsigned int);
int    read(int, char *, unsigned int);
int    setmode(int, int);
int    sopen(const char *, unsigned int, unsigned int, ...);
int   _stdio_stdopen(int);
long   tell(int);
mode_t umask(mode_t);
int    write(int, const char *, unsigned int);

/*
 * ANSI and POSIX functions prototyped in stdio.h but also declared here
 * for Microsoft(reg.) compatibility:
 */
#ifndef _io_stdio
#define _io_stdio
int    remove(const char *);
int    rename(const char *, const char *);
int    unlink(const char *);
#endif

/*
 * Prototypes of functions not supported by C Code Builder(TM) Libraries:
 */
int    _pipe(int *, unsigned int, int);

#endif /* _ioh */

