/* limits.h - compiler limits
 * $Version: 1.9 $
 * Copyright 1988,89,90,91 Intel Corporation, ALL RIGHTS RESERVED.
 */

#ifndef _limitsh
#define _limitsh

#define CHAR_BIT                   8    /* Number of bits in a byte          */
#define UCHAR_MAX                255u   /* Maximum value of an unsigned char */
#define SCHAR_MAX                127    /* Maximum value for a signed char   */
#define SCHAR_MIN              (-128)   /* Minimum value of a signed char    */
                                        /*   (-SCHAR_MAX - 1)                */

#if '\xFF' < 0
#define CHAR_MAX           SCHAR_MAX    /* Maximum value of a 'plain' char   */
#define CHAR_MIN           SCHAR_MIN    /* Minimum value of a 'plain' char   */
#else
#define _CHAR_UNSIGNED
#define CHAR_MAX           UCHAR_MAX    /* Maximum value of a 'plain' char   */
#define CHAR_MIN                   0    /* Minimum value of a 'plain' char   */
#endif

#define USHRT_MAX              65535u   /* Maximum value of an unsigned short*/
#define SHRT_MAX               32767    /* Maximum value of a short int      */
#define SHRT_MIN             (-32768)   /* Minimum value of a short int      */
                                        /*  (-SHRT_MAX - 1)                  */
#define ULONG_MAX         4294967295u   /* Maximum value of an unsigned long */
#define LONG_MAX          2147483647L   /* Maximum value of a long int       */
#define LONG_MIN     (-2147483647L-1)   /* Minimum value of a long int       */
                                        /*  (-LONG_MAX - 1)                  */
#define INT_MAX             LONG_MAX    /* Maximum value of an int           */
#define INT_MIN             LONG_MIN    /* Minimum value of an int           */
#define UINT_MAX           ULONG_MAX    /* Maximum value of an unsigned int  */

#define MB_LEN_MAX                 1

#endif /* _limitsh */
