/* setjmp.h - set jump definitions
 * $Version: 1.7 $
 * Copyright 1988,89,90,91 Intel Corporation, ALL RIGHTS RESERVED
 */

#ifndef _setjmph
#define _setjmph

#define _JBLEN  9
typedef int jmp_buf[_JBLEN];

/*
 * Function prototypes:
 */
void longjmp(jmp_buf, int);
int setjmp(jmp_buf);

#endif /* _setjmph */

