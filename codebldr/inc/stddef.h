/* stddef.h - contains standard definitions
 * $Version: 1.7 $
 * Copyright 1988,89,90,91 Intel Corporation, ALL RIGHTS RESERVED.
 */

#ifndef _stddefh
#define _stddefh

#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef _ptrdiff_t
typedef int ptrdiff_t;
#define _ptrdiff_t
#define _PTRDIFF_T_DEFINED
#endif

#ifndef _size_t
typedef unsigned size_t;
#define _size_t
#define _SIZE_T_DEFINED
#endif

#ifndef _wchar_t
#define _wchar_t
typedef char wchar_t;
#endif

#define offsetof(s_type, memb)  ((size_t)(char *)&((s_type *)0)->memb)
#define NUL '\0'                                        /* String terminator */

#endif /* _stddefh */

