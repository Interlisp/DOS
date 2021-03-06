/* timeb.h -
 * $Version: 1.4 $
 * Copyright 1990,91 Intel Corporation, ALL RIGHTS RESERVED.
 */

#ifndef _timebh
#define _timebh

#ifndef _time_t
typedef long time_t;
#define _TIME_T_DEFINED
#define _time_t
#endif

/*
 * Structure returned by the ftime() sytem call:
 */
#ifndef _timeb
struct timeb {
    time_t         time;
    unsigned short millitm;
    short          timezone;
    short          dstflag;
};
#define _TIMEB_DEFINED
#define _timeb
#endif

/*
 * Function prototypes:
 */
void ftime(struct timeb *);

#endif /* _timebh */
