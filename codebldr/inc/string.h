/* string.h - string function prototypes
 * $Version: 1.13 $
 * Copyright 1988,89,90,91 Intel Corporation, ALL RIGHTS RESERVED.
 */

#ifndef _stringh
#define _stringh


#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef _size_t
typedef unsigned size_t;
#define _SIZE_T_DEFINED
#define _size_t
#endif

/*
 * Function prototypes:
 */
char *_strerror(const char *);
void  *memccpy(void *, const void *, int, int);
void  *memchr(const void *, int, size_t);
int    memcmp(const void*, const void*, size_t);
void  *memcpy(void*, const void*, size_t);
int    memicmp(const void *, const void *, unsigned);
void  *memmove(void *, const void *, size_t);
void  *memset(void*, int, size_t);
void   movedata(unsigned, unsigned, unsigned, unsigned, unsigned);
char  *strcat(char *, const char *);
char  *strchr(const char *, int);
int    strcmp(const char *, const char *);
int    strcmpi(const char *, const char *);
int    strcoll(const char *, const char *);
char  *strcpy(char*, const char*);
size_t strcspn(const char *, const char *);
char  *strdup(const char *);
char  *strerror(int);
int    stricmp(const char *, const char *);
size_t strlen(const char *);
char  *strlwr(char *);
char  *strncat(char *, const char *, size_t);
int    strncmp(const char *, const char *, size_t);
char  *strncpy(char *, const char *, size_t);
int    strnicmp(const char *, const char *, size_t);
char  *strnset(char *, int, size_t);
char  *strpbrk(const char *, const char *);
char  *strrchr(const char *, int);
char  *strrev(char *);
char  *strset(char *, int);
size_t strspn(const char *, const char *);
char  *strstr(const char *, const char *);
char  *strtok(char *, const char *);
char  *strupr(char *);
size_t strxfrm(char *, const char *, size_t);

/*
 * String macro declarations:
 */
#define _fmemccpy(_a,_b,_c,_d)  memccpy(_a,_b,_c,_d)
#define _fmemchr(_a,_b,_c)      memchr(_a,_b,_c)
#define _fmemcmp(_a,_b,_c)      memcmp(_a,_b,_c)
#define _fmemcpy(_a,_b,_c)      memcpy(_a,_b,_c)
#define _fmemicmp(_a,_b,_c)     memicmp(_a,_b,_c)
#define _fmemmove(_a,_b,_c)     memmove(_a,_b,_c)
#define _fmemset(_a,_b,_c)      memset(_a,_b,_c)
#define _fstrcat(_a,_b)         strcat(_a,_b)
#define _fstrchr(_a,_b)         strchr(_a,_b)
#define _fstrcmp(_a,_b)         strcmp(_a,_b)
#define _fstrcmpi(_a,_b)        strcmpi(_a,_b)
#define _fstrcpy(_a,_b)         strcpy(_a,_b)
#define _fstrcspn(_a,_b)        strcspn(_a,_b)
#define _fstrdup(_a)            strdup(_a)
#define _fstricmp(_a,_b)        stricmp(_a,_b)
#define _fstrlen(_a)            strlen(_a)
#define _fstrlwr(_a)            strlwr(_a)
#define _fstrncat(_a,_b,_c)     strncat(_a,_b,_c)
#define _fstrncmp(_a,_b,_c)     strncmp(_a,_b,_c)
#define _fstrncpy(_a,_b,_c)     strncpy(_a,_b,_c)
#define _fstrnicmp(_a,_b,_c)    strnicmp(_a,_b,_c)
#define _fstrnset(_a,_b,_c)     strnset(_a,_b,_c)
#define _fstrpbrk(_a,_b)        strpbrk(_a,_b)
#define _fstrrchr(_a,_b)        strrchr(_a,_b)
#define _fstrrev(_a)            strrev(_a)
#define _fstrset(_a,_b)         strset(_a,_b)
#define _fstrspn(_a,_b)         strspn(_a,_b)
#define _fstrstr(_a,_b)         strstr(_a,_b)
#define _fstrtok(_a,_b)         strtok(_a,_b)
#define _fstrupr(_a)            strupr(_a)
#define _nstrdup(_a)            strdup(_a)

/*
 * Un-comment this macro if you wish to map the  movedata()  function
 * onto the  memcpy()  function:
 *
 * #define movedata(_a, _b, _c, _d, _e) \
 *                   memcpy((void *)((_c<<4)+_d), (void *)((_a<<4)+_b), _e)
 */

/*
 * Compiler built-in functions:
 */
#ifndef _stringh_builtin
#define _stringh_builtin

#pragma _builtin_("_memcpy_"==64)
void *_memcpy_(void*, const void*, size_t);
#define memcpy(x,y,z) _memcpy_(x,y,z)

#pragma _builtin_("_memset_"==65)
void *_memset_(void*, int, size_t);
#define memset(x,y,z) _memset_(x,y,z)

#pragma _builtin_("_strcpy_"==71)
char *_strcpy_(char*, const char*);
#define strcpy(x,y)  _strcpy_(x,y)

#endif /* _stringh_builtin */

#endif /* _stringh */
