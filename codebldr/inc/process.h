/* process.h - process control function prototypes
 * $Version: 1.15 $
 * Copyright 1988,89,90,91 Intel Corporation, ALL RIGHTS RESERVED.
 */

#ifndef _processh
#define _processh

#ifndef _typesh
#include <sys/types.h>
#endif

#define P_WAIT          0
#define P_NOWAIT        1
#define P_OVERLAY       2
#define OLD_P_OVERLAY   2
#define P_NOWAITO       3
#define P_DETACH        4

#define WAIT_CHILD      0
#define WAIT_GRANDCHILD 0


/*
 * Function prototypes:
 */
void    _c_exit(void);
void    _cexit(void);
int     _doexec(int, int, int, int, int, int, char *, char *, char *,
            char *, char *, char *);
int     _exec(const char *, const char **, const char **, int);
int      execl(const char *, const char *, ...);
int      execle(const char *, const char *, ...);
int      execlp(const char *, const char *, ...);
int      execlpe(const char *, const char *,...);
int      execv(const char *, const char **);
int      execve(const char *, const char **, const char **);
int      execvp(const char *, const char **);
int      execvpe(const char *, const char **, const char **);
pid_t    getpid(void);
unsigned sleep(unsigned);
int     _spawn(int, const char *, const char **, const char **, int);
int      spawnl(int, const char *, const char *, ...);
int      spawnle(int, const char *, const char *,...);
int      spawnlp(int, const char *, const char *, ...);
int      spawnlpe(int, const char *, const char *, ...);
int      spawnv(int, const char *, const char **);
int      spawnve(int, const char *, const char **, const char **);
int      spawnvp(int, const char *, const char **);
int      spawnvpe(int, const char *, const char **, const char **);

/*
 * ANSI and POSIX functions also prototyped in io.h
 * for Microsoft(reg.) compatibility:
 */
#ifndef _process_stdlib
#define _process_stdlib
void  abort(void);
void _exit(int);
void  exit(int);
int   system(const char *);
#endif

/*
 * Prototypes of functions not supported by C Code Builder(TM) Libraries:
 */
int cwait(int *, int, int);
int wait(int *);

#endif /* _processh */

