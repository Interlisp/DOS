/* search.h - searching and sorting function prototypes
 * $Version: 1.8 $
 * Copyright 1988,89,90,91 Intel Corporation, ALL RIGHTS RESERVED.
 */

#ifndef _searchh
#define _searchh

#ifndef _size_t
typedef unsigned size_t;
#define _size_t
#define _SIZE_T_DEFINED
#endif

/*
 * Function prototypes:
 */
void *lfind(const void *, const void *, unsigned *, unsigned,
         int (*)(const void *, const void *));
void *lsearch(const void *, void *, unsigned *, unsigned,
         int (*)(const void *, const void *));

/*
 * ANSI functions also prototyped in stdlib.h
 * for Microsoft(reg.) compatibility:
 */
#ifndef _search_stdlib
#define _search_stdlib
void *bsearch(const void *, const void *, size_t, size_t,
         int (*)(const void *, const void *));
void  qsort(void *, size_t, size_t, int (*)(const void *, const void *));
#endif

#endif /* _searchh */

