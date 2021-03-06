/* stdio.h - standard I/O header file
 * $Version: 1.25 $
 * Copyright 1988,89,90,91 Intel Corporation, ALL RIGHTS RESERVED.
 */

#ifndef _stdioh
#define _stdioh

#ifndef NULL
#define NULL             ((void *)0)
#endif

#ifndef _fpos_t
typedef unsigned long fpos_t;
#define _FPOS_T_DEFINED
#define _fpos_t
#endif

#ifndef _size_t
typedef unsigned size_t;
#define _SIZE_T_DEFINED
#define _size_t
#endif

#ifndef _va_list
typedef char *va_list;
#define _VA_LIST_DEFINED
#define _va_list
#endif

#define BUFSIZ          4096
#define FOPEN_MAX       20
#define SYS_OPEN        FOPEN_MAX
#define _NFILE          FOPEN_MAX
#define TMP_MAX         4096
#define FILENAME_MAX    127
#define EOF             (-1)

#define L_tmpnam        13
#define P_tmpdir        "\\"

#define _IOREAD         0x01
#define _IOWRT          0x02

#define _IOFBF          0x00    /* Neither line nor char buffering */
#define _IOLBF          0x40
#define _IONBF          0x04

#define _IOMYBUF        0x08
#define _IOEOF          0x10
#define _IOERR          0x20
#define _IOSTRG         0x40
#define _IORW           0x80

#define _IODIRTY        0x200
#define _IO_STD_STREAM  0x1000

/* seek */
#ifndef SEEK_SET
#define SEEK_SET        0
#define SEEK_CUR        1
#define SEEK_END        2
#endif

#ifndef _reenth
#pragma align (_iobuf)
#endif

#ifndef _FILE_DEFINED
struct _iobuf {
    unsigned char    *_ptr;
    int               _cnt;
    unsigned char    *_base;
    int               _flag;
    int               _fd;                     /* File descriptor number */
    int               _size;                   /* File buffer size */
    char             *_temp_name;              /* Temporary file name */
#ifdef _MT
    void             *_sem;                    /* Semaphore */
#endif /* _MT */
    struct            _iobuf *_next_stream;    /* Pointer to the next stream */
};
typedef struct _iobuf FILE;
#define _FILE_DEFINED
#endif

#ifdef _MT
#include <reent.h>
#else
extern FILE    _stdin;     /* stdin stream */
extern FILE    _stdout;    /* stdout stream */
extern FILE    _stderr;    /* stderr stream */
extern FILE    _stdaux;    /* stdaux stream */
extern FILE    _stdprn;    /* stdprn stream */

#define stdin     ((FILE *)&_stdin)
#define stdout    ((FILE *)&_stdout)
#define stderr    ((FILE *)&_stderr)
#define stdaux    ((FILE *)&_stdaux)
#define stdprn    ((FILE *)&_stdprn)
#endif /* _MT */

/*
 * Function prototypes:
 */
void     clearerr(FILE *);
int      fclose(FILE *);
int      fcloseall(void);
FILE    *fdopen(int, const char *);
int      feof(FILE *);
int      ferror(FILE *);
int      fflush(FILE *);
int      fgetc(FILE *);
int      fgetchar(void);
int      fgetpos(FILE *, fpos_t *);
char    *fgets(char *, int, FILE *);
int      fileno(FILE *);
int      flushall(void);
FILE    *fopen(const char *, const char *);
int      fprintf(FILE *, const char *, ...);
int      fputc(int, FILE *);
int      fputchar(int);
int      fputs(const char *, FILE *);
size_t   fread(void *, size_t, size_t, FILE *);
FILE    *freopen(const char *, const char *, FILE *);
int      fscanf(FILE *, const char *, ...);
int      fseek(FILE *, long int, int);
int      fsetpos(FILE *, const fpos_t *);
FILE   *_fsopen(const char *, const char *, int);
long int ftell(FILE *);
size_t   fwrite(const void *, size_t, size_t, FILE *);
int      getc(FILE *);
int      getchar(void);
char    *gets(char *);
int      getw(FILE *);
int     _init_handle_count ( int );
int      printf(const char *, ...);
int      putc(int, FILE *);
int      putchar(int);
int      puts(const char *);
int      putw(int, FILE *);
void     rewind(FILE *);
int      rmtmp(void);
int      scanf(const char *, ...);
void     setbuf(FILE *, char *);
int      setvbuf(FILE *, char *, int, size_t);
int      sprintf(char *, const char *, ...);
int      sscanf(const char *, const char *, ...);
char    *tempnam(char *, const char *);
FILE    *tmpfile(void);
char    *tmpnam(char *);
int      ungetc(int, FILE *);
int      vfprintf(FILE *, const char *, va_list);
int      vprintf( const char *, va_list);
int      vsprintf(char *, const char *, va_list);

/*
 * ANSI and POSIX functions also prototyped in io.h
 * for Microsoft(reg.) compatibility:
 */
#ifndef _io_stdio
#define _io_stdio
int      remove(const char *);
int      rename(const char *, const char *);
int      unlink(const char *);
#endif

/*
 * ANSI function also prototyped in stdlib.h
 * for Microsoft compatibility:
 */
#ifndef _stdlib_stdio
#define _stdlib_stdio
void     perror(const char *);
#endif

/*
 * Prototypes of functions not supported by C Code Builder(TM) Libraries
 */
int     _pclose(FILE *);
FILE   *_popen(const char *, const char *);

/*
 * Macro implementation of stdio functions:
 */
#define feof(_f)     ((_f)->_flag & _IOEOF)
#define ferror(_f)   ((_f)->_flag & _IOERR)
#define fgetchar()   fgetc(stdin)
#define fgetc(_s)    getc(_s)
#define fileno(_f)   ((_f)->_fd)
#define fputc(_c,_s) putc(_c,_s)
#define getchar()    getc(stdin)
#define putchar(_c)  putc((_c),stdout)
#define _ungetc_

#define getc(_s)     (((((_s)->_cnt) > 0) && (((_s)->_cnt) < ((_s)->_size))) \
? (((_s)->_cnt--), (unsigned char)(*(_s)->_ptr++)) : ((getc)(_s)))
 
#define putc(_c,_s)  (((((_s)->_cnt) > 0) && (((_s)->_cnt) < ((_s)->_size))) \
? (((_s)->_cnt--), (*((_s)->_ptr++)=(unsigned char)(_c))) : ((putc)(_c,_s)))

#endif /* _stdioh */
