/* stdlib.h - standard utility functions
 * $Version: 1.19 $
 * Copyright 1988,89,90,91 Intel Corporation, ALL RIGHTS RESERVED
 */

#ifndef _stdlibh
#define _stdlibh

#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef _size_t
#define _size_t
typedef unsigned size_t;
#endif

#ifndef _wchar_t
#define _wchar_t
typedef char wchar_t;
#endif

#ifndef _div_t
typedef struct {
   int quot;
   int rem;
} div_t;

typedef struct {
   long quot;
   long rem;
} ldiv_t;

#define _div_t
#define _DIV_T_DEFINED
#endif

typedef int (*onexit_t)(void);
#define _ONEXIT_T_DEFINED

#define RAND_MAX        0x7FFF
#define MB_CUR_MAX      1
#define EXIT_FAILURE    1
#define EXIT_SUCCESS    0

#define _MAX_PATH       260             /* Max length of full pathname       */
#define _MAX_DRIVE      3               /* Max length of drive component     */
#define _MAX_DIR        256             /* Max length of path component      */
#define _MAX_FNAME      256             /* Max length of file name component */
#define _MAX_EXT        256             /* Max length of extension component */

#define DOS_MODE        0               /* Real address mode                 */
#define OS2_MODE        1               /* Protected address mode            */

#ifdef _MT
#ifndef _reenth
#include <reent.h>
#endif
#endif /* _MT */

#ifndef errno
#ifdef _MT
#define errno       ((_thread_ptr()->errno))    /* Library error variable */
#else
extern	int			 errno;
#endif /* _MT */
#endif /* _errno */

#ifdef _MT
#define _doserrno  ((_thread_ptr()->__doserrno)) /* DOS system error variable */
#else
extern	int			 _doserrno;
#endif /* _MT */

extern char          **environ;         /* Environment table ptr             */
extern int            _fileinfo;        /* For spawn, open file info mode    */
extern int            _fmode;           /* Default file translation mode     */
extern unsigned char  _osmajor;         /* Operating system version          */
extern unsigned char  _osminor;         /* Operating system point release    */
extern unsigned char  _osmode;          /* DOS_MODE or OS2_MODE              */
extern unsigned int   _psp;             /* Address of Program Segment Prefix */
extern char           *sys_errlist[];   /* Error message table for perror    */
extern int             sys_nerr;        /* Number of sys_errlist entries     */

/*
 * Function prototypes:
 */
int           atexit(void (*)(void));
int           atoi(const char *);
long          atol(const char *);
void         *calloc(size_t, size_t);
div_t         div(int, int);
char         *ecvt(double, int, int *, int *);
char         *fcvt(double, int, int *, int *);
void          free(void *);
char        *_fullpath(char *, const char *, size_t);
char         *gcvt(double, int, char *);
char         *getenv(const char *);
char         *itoa(int, char *, int);
char         *itoh(int, char *);
ldiv_t        ldiv(long, long);
char         *ltoa(long, char *, int);
char         *ltoh(unsigned long, char *);
char         *ltos(long int, char *, int);
void         _makepath(char *, const char *, const char *,
                       const char *, const char *);
void         *malloc(size_t);
int           mblen(const char *, size_t);
size_t        mbstowcs(wchar_t *, const char *, size_t);
int           mbtowc(wchar_t *, const char *, size_t);
onexit_t      onexit(onexit_t);
int           putenv(const char *);
int           rand(void);
void         *realloc(void *, size_t);
void         *sbrk(unsigned);
void         _searchenv(const char *, const char *, char *);
void         _splitpath(const char *, char *, char *, char *, char *);
void          srand(unsigned int);
double        strtod(const char *, char **);
long          strtol(const char *, char **, int);
long double  _strtold(const char *, char **);
unsigned long strtoul(const char *, char **, int);
void          swab(const char *, char *, int);
void          swab16(const char *, char *, int);
char         *ultoa(unsigned long, char *, int);
size_t        wcstombs(char *, const wchar_t *, size_t);
int           wctomb(char *, wchar_t);

#ifndef tolower
int            tolower(int);
#endif
#ifndef toupper
int            toupper(int);
#endif

/*
 * ANSI function also prototyped in math.h
 * for Microsoft(reg.) compatibility:
 */
#ifndef _math_stdlib
#define _math_stdlib
int           abs(int);
double        atof(const char *);
long double  _atold(const char *);
long          labs(long);
#define _atold(_a)     ( (long double) atof(_a) )
#endif

/*
 * ANSI and POSIX functions also prototyped in process.h
 * for Microsoft compatibility:
 */
#ifndef _process_stdlib
#define _process_stdlib
void          abort(void);
void         _exit(int);
void          exit(int);
int           system(const char *);
#endif

/*
 * ANSI functions also prototyped in search.h
 * for Microsoft compatibility:
 */
#ifndef _search_stdlib
#define _search_stdlib
void         *bsearch(const void *, const void *, size_t, size_t,
                      int (*)(const void *, const void *));
void          qsort(void *, size_t, size_t, int (*)(const void *,
                    const void *));
#endif

/*
 * ANSI function also prototyped in stdio.h
 * for Microsoft compatibility:
 */
#ifndef _stdlib_stdio
#define _stdlib_stdio
void          perror(const char *);
#endif

/*
 * In-line implementation of functions:
 */
unsigned long   _lrotl(unsigned long, int);
unsigned long   _lrotr(unsigned long, int);
unsigned int    _rotl(unsigned int, int);
unsigned int    _rotr(unsigned int, int);
unsigned short  _srotl(unsigned short, int);
unsigned short  _srotr(unsigned short, int);

#define _lrotl(_a,_b)  _word_rol((unsigned int)(_a),(unsigned int)(_b))
#define _lrotr(_a,_b)  _word_ror((unsigned int)(_a),(unsigned int)(_b))
#define _rotl(_a,_b)   _word_rol((unsigned int)(_a),(unsigned int)(_b))
#define _rotr(_a,_b)   _word_ror((unsigned int)(_a),(unsigned int)(_b))
#define _srotl(_a,_b)  _hword_rol((unsigned short)(_a),(unsigned int)(_b))
#define _srotr(_a,_b)  _hword_ror((unsigned short)(_a),(unsigned int)(_b))

/*
 * Compiler built-in functions:
 */
#ifndef _stdlibh_builtin
#define _stdlibh_builtin

#pragma _builtin_("_abs_"==51)
int _abs_(int);
#define abs(x) _abs_(x)

#pragma _builtin_("_labs_"==52)
long _labs_(long);
#define labs(x) _labs_(x)

#pragma _builtin_("_hword_rol"==76)
unsigned short  _hword_rol(unsigned short, unsigned int);

#pragma _builtin_("_hword_ror"==77)
unsigned short  _hword_ror(unsigned short, unsigned int);

#pragma _builtin_("_word_rol"==78)
unsigned int    _word_rol(unsigned int, unsigned int);

#pragma _builtin_("_word_ror"==79)
unsigned int    _word_ror(unsigned int, unsigned int);

#endif /* _stdlibh_builtin */

/*
 * Macro definitions:
 */
#define max(a,b) ( ((a) > (b)) ? (a) : (b) )
#define min(a,b) ( ((a) < (b)) ? (a) : (b) )
#define _strtold(_a, _b) ( (long double) strtod(_a, _b) )

#endif  /* _stdlibh */
