/* assert.h - define the assert macro
 * $Version: 1.9 $
 * Copyright 1988,89,90,91 Intel Corporation, ALL RIGHTS RESERVED.
 */

#undef assert

#ifndef NDEBUG

void _assert(void *, void *, unsigned);

#define assert(_exp_) \
   ( (_exp_) ? (void) 0 : _assert(#_exp_, __FILE__, __LINE__) )


#else

#define assert(ignore) ((void)0)

#endif /* NDEBUG */

