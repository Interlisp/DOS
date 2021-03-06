/* malloc.h - memory allocation functions
 * $Version: 1.17 $
 * Copyright 1990,91 Intel Corporation, ALL RIGHTS RESERVED.
 */

#ifndef _malloch
#define _malloch

#ifndef _dosh
#include <dos.h>
#endif

#ifndef _stdlibh
#include <stdlib.h>
#endif

#ifndef _size_t
typedef unsigned size_t;
#define _size_t
#define _SIZE_T_DEFINED
#endif

#ifndef __heapinfo
#pragma align (_HEAPINFO)
typedef struct _heapinfo {
    int  * _pentry;
    size_t _size;
    int    _useflag;
    } _HEAPINFO;
#define __heapinfo
#define _HEAPINFO_DEFINED
#endif

/* Values used by _heapchk, _heapset, _heapwalk */
#define _USEDENTRY       1
#define _FREEENTRY       0
#define _HEAPEMPTY     (-1)
#define _HEAPOK        (-2)
#define _HEAPBADBEGIN  (-3)
#define _HEAPBADNODE   (-4)
#define _HEAPEND       (-5)
#define _HEAPBADPTR    (-6)

/* Largest heap request that can be filled */
#define _HEAP_MAXREQ    0xFFFFFFF

#define _NULLSEG             ( (int)0 )
#define _NULLOFF             ( (void *)0xFFFFFFFF )

extern unsigned int _amblksiz;

/*
 * Function prototypes:
 */
void      *alloca(size_t);
void     *_expand(void *, size_t);
unsigned  _freect(size_t);
int       _heapadd(void *, size_t);
int       _heapchk(void);
int       _heapmin(void);
int       _heapset(unsigned int);
int       _heapwalk(_HEAPINFO *);
size_t    _memavl(void);
size_t    _memmax(void);
size_t    _msize(void *);


/*
 * Prototypes of function not supported by C Code Builder(TM) Libraries:
 */
size_t     stackavail(void);

/*
 * Map the based, far, huge, and near malloc functions onto their
 * corresponding generic functions (these functions are not needed
 * in a 32-bit environment):
 */
#define _bcalloc(_a,_b,_c)   calloc(_b,_c)
#define _bexpand(_a,_b,_c)  _expand(_b,_c)
#define _bfree(_a,_b)        free(_b)
#define _bfreeseg(_a)       (0)
#define _bheapadd(_a,_b,_c) _heapadd(_b,_c)
#define _bheapchk(_a)       _heapchk()
#define _bheapmin(_a)       _heapmin()
#define _bheapseg(_a)       (_dataseg)
#define _bheapset(_a,_b)    _heapset(_b)
#define _bheapwalk(_a,_b)   _heapwalk(_b)
#define _bmalloc(_a,_b)      malloc(_b)
#define _bmsize(_a,_b)      _msize(_b)
#define _brealloc(_a,_b,_c)  realloc(_b,_c)

#define _fcalloc(_a,_b)      calloc(_a,_b)
#define _fexpand(_a,_b)     _expand(_a,_b)
#define _ffree(_a)           free(_a)
#define _fheapchk           _heapchk
#define _fheapmin           _heapmin
#define _fheapset(_a)       _heapset(_a)
#define _fheapwalk(_a)      _heapwalk(_a)
#define _fmalloc(_a)         malloc(_a)
#define _fmsize(_a)         _msize(_a)
#define _frealloc(_a,_b)     realloc(_a,_b)

#define  halloc(_a,_b)       calloc((size_t)(_a),_b)
#define  hfree(_a)           free(_a)

#define _ncalloc(_a,_b)      calloc(_a,_b)
#define _nexpand(_a,_b)     _expand(_a,_b)
#define _nfree(_a)           free(_a)
#define _nheapchk           _heapchk
#define _nheapmin           _heapmin
#define _nheapset(_a)       _heapset(_a)
#define _nheapwalk(_a)      _heapwalk(_a)
#define _nmalloc(_a)         malloc(_a)
#define _nmsize(_a)         _msize(_a)
#define _nrealloc(_a,_b)     realloc(_a,_b)

#pragma _builtin_("_alloca_"==83)
void   *_alloca_(size_t);
#define  alloca(_x) _alloca_(_x)

#endif /* _malloch */
