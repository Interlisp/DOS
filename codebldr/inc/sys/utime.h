/* utime.h - basic struct for utime()
 * $Version: 1.6 $
 * Copyright 1988,89,90,91 Intel Corporation, ALL RIGHTS RESERVED.
 */

#ifndef _utimeh
#define _utimeh

#ifndef _time_t
typedef long time_t;
#define _TIME_T_DEFINED
#define _time_t
#endif

#pragma align (utimbuf)
struct utimbuf {
  time_t actime;            /* Time of last access - not available under DOS */
  time_t modtime;           /* Time of last modification                     */
};
#define _UTIMBUF_DEFINED

/*
 * Function prototypes:
 */
int utime(const char *, struct utimbuf *);

#endif /* _utimeh */
