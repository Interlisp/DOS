/* float.h - floating point constants and floating-point unit defintions
 * $Version: 1.21 $
 * Copyright 1988,89,90,91 Intel Corporation, ALL RIGHTS RESERVED.
 */

#ifndef _floath
#define _floath

#define DBL_DIG          15    /* Maximum decimal digits of double precision */
#define DBL_EPSILON      2.2204460492503131e-16
#define DBL_MANT_DIG     53
#define DBL_MAX          1.7976931348623157e+308
#define DBL_MAX_10_EXP   308
#define DBL_MAX_EXP      1024
#define DBL_MIN          2.2250738585072014e-308
#define DBL_MIN_10_EXP (-307)
#define DBL_MIN_EXP    (-1021)
#define DBL_RADIX        2             /* Radix of the floating-point number */
                                       /* e.g., a binary machine             */
#define DBL_ROUNDS       1

#define FLT_DIG          6      /* Maximum decimal digits of float precision */
#define FLT_EPSILON      1.19209290e-07F
#define FLT_MANT_DIG     24
#define FLT_MAX          3.40282347e+38F
#define FLT_MAX_10_EXP   38
#define FLT_MAX_EXP      128
#define FLT_MIN          1.17549435e-38F
#define FLT_MIN_10_EXP (-37)
#define FLT_MIN_EXP    (-125)
#define FLT_RADIX        2             /* Radix of the floating-point number */
                                       /* e.g., a binary machine             */
#define FLT_ROUNDS       1
#define FLT_GUARD        0
#define FLT_NORMALIZE    0

#define LDBL_DIG         15                     /* Maximum decimal digits of */
                                                /* long double precision     */
#define LDBL_EPSILON     1.110223024625156e-16
#define LDBL_MANT_DIG    53
#define LDBL_MAX         1.7976931348623157e+308
#define LDBL_MAX_10_EXP  308
#define LDBL_MAX_EXP     1023
#define LDBL_MIN          2.2250738585072014e-308
#define LDBL_MIN_10_EXP (-307)
#define LDBL_MIN_EXP   (-1021)
#define LDBL_RADIX       2             /* Radix of the floating-point number */
                                       /* e.g., a binary machine             */
#define LDBL_ROUNDS      1

/*
 * Numeric coprocessor configuration constants:
 */
#define MCW_EM              0x003f      /* Interrupt Exception Masks         */
#define EM_INVALID          0x0001      /*   Invalid                         */
#define EM_DENORMAL         0x0002      /*   Denormal                        */
#define EM_ZERODIVIDE       0x0004      /*   Zero divide                     */
#define EM_OVERFLOW         0x0008      /*   Overflow                        */
#define EM_UNDERFLOW        0x0010      /*   Underflow                       */
#define EM_INEXACT          0x0020      /*   Inexact (precision)             */

#define MCW_IC              0x1000      /* Infinity Control                  */
#define IC_AFFINE           0x1000      /*   Affine                          */
#define IC_PROJECTIVE       0x0000      /*   Projective                      */

#define MCW_RC              0x0c00      /* Rounding Control                  */
#define RC_CHOP             0x0c00      /*   Chop                            */
#define RC_UP               0x0800      /*   Up                              */
#define RC_DOWN             0x0400      /*   Down                            */
#define RC_NEAR             0x0000      /*   Near                            */

#define MCW_PC              0x0300      /* Precision Control                 */
#define PC_24               0x0000      /*    24 bits                        */
#define PC_53               0x0200      /*    53 bits                        */
#define PC_64               0x0300      /*    64 bits                        */

#define CW_DEFAULT  (IC_AFFINE + RC_NEAR + PC_64 + EM_DENORMAL + \
                     EM_UNDERFLOW + EM_INEXACT)

/*
 * User status word:
 */
#define SW_INVALID          0x0001      /* Invalid                           */
#define SW_DENORMAL         0x0002      /* Denormal                          */
#define SW_ZERODIVIDE       0x0004      /* Zero divide                       */
#define SW_OVERFLOW         0x0008      /* Overflow                          */
#define SW_UNDERFLOW        0x0010      /* Underflow                         */
#define SW_INEXACT          0x0020      /* Inexact (precision)               */
#define SW_UNEMULATED       0x0040      /* Unemulated instruction            */
#define SW_SQRTNEG          0x0080      /* Square root of a neg number       */
#define SW_STACKOVERFLOW    0x0200      /* Floating-point stack overflow     */
#define SW_STACKUNDERFLOW   0x0400      /* Floating-point stack underflow    */

/*
 * Floating-point error signals and return codes:
 */
#define FPE_INVALID         0x81
#define FPE_DENORMAL        0x82
#define FPE_ZERODIVIDE      0x83
#define FPE_OVERFLOW        0x84
#define FPE_UNDERFLOW       0x85
#define FPE_INEXACT         0x86
#define FPE_UNEMULATED      0x87
#define FPE_SQRTNEG         0x88
#define FPE_STACKOVERFLOW   0x8a
#define FPE_STACKUNDERFLOW  0x8b

#define FPE_EXPLICITGEN     0x8c                         /* Raise( SIGFPE ); */

/*
 * Function prototypes:
 */

#ifndef _87_functions
#define _87_functions
unsigned int _clear87(void);
unsigned int _control87(unsigned int, unsigned int);
void         _fpreset(void);
unsigned int _status87(void);
#endif

#endif /* _floath */

