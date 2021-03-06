/* time.h - basic struct for time(), asctime(), localtime()
 * $Version: 1.10 $
 * Copyright 1988,89,90,91 Intel Corporation, ALL RIGHTS RESERVED.
 */

#ifndef _timeh
#define _timeh

extern char *tzname[2];
extern long  timezone;
extern long  altzone;
extern int   daylight;


#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef _clock_t
typedef unsigned long clock_t;
#define _CLOCK_T_DEFINED
#define _clock_t
#endif

#ifndef _size_t
typedef unsigned size_t;
#define _SIZE_T_DEFINED
#define _size_t
#endif

#ifndef _time_t
typedef long time_t;
#define _TIME_T_DEFINED
#define _time_t
#endif

#define CLOCKS_PER_SEC 1000
#define CLK_TCK        CLOCKS_PER_SEC

#ifndef _tm
#ifndef _reenth
#pragma align (tm)
#endif
struct tm {
    int tm_sec;                         /* Seconds (0-59)                    */
    int tm_min;                         /* Minutes (0-59)                    */
    int tm_hour;                        /* Hours (0-23)                      */
    int tm_mday;                        /* Days (1-31)                       */
    int tm_mon;                         /* Months (0-11)                     */
    int tm_year;                        /* Year (minus 1900)                 */
    int tm_wday;                        /* Day of week (Sunday = 0)          */
    int tm_yday;                        /* Day of year (0-365)               */
    int tm_isdst;                       /* Non-zero if day-light savings time*/
};
#define _TM_DEFINED
#define _tm
#endif

/*
 * Function prototypes:
 */
char      *asctime(const struct tm *);
clock_t    clock(void);
char      *ctime(const time_t *);
double     difftime(time_t, time_t);
struct tm *gmtime(const time_t *);
struct tm *localtime(const time_t *);
time_t     mktime(struct tm *);
char     *_strdate(char *);
size_t     strftime(char *, size_t, const char *, const struct tm *);
char     *_strtime(char *);
time_t     time(time_t *);
void       tzset(void);

#endif /* _timeh */
