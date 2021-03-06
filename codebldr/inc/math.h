/* math.h - math and trig definitions
 * $Version: 1.21 $
 * Copyright 1988,89,90,91 Intel Corporation, ALL RIGHTS RESERVED.
 */

#ifndef _mathh
#define _mathh

#ifndef _errnoh
#include <errno.h>
#endif

#ifndef HUGE_VAL
#define HUGE_VAL    1.797693134862315e308   /* DBL_MAX */
#define _LHUGE_VAL  1.797693134862315e308   /* LDBL_MAX */
#endif

/*
 * Exception types for matherr():
 */
#define DOMAIN      1                   /* Argument domain error             */
#define SING        2                   /* Singularity error                 */
#define OVERFLOW    3                   /* Function overflow error           */
#define UNDERFLOW   4                   /* Function underflow error          */
#define TLOSS       5                   /* Total loss of precision           */
#define PLOSS       6                   /* Partial loss of precision         */

#ifndef _complex
#pragma align (complex)
struct complex {
  double x;                             /* Real part                         */
  double y;                             /* Imaginary part                    */
};

#define _complex
#define _COMPLEX_DEFINED
#endif

#ifndef __complexl
#pragma align (_complexl)
struct _complexl {
  long double x;                        /* Real part                         */
  long double y;                        /* Imaginary part                    */
};

#define __complexl
#define _LD_COMPLEX_DEFINED
#endif

#ifndef _exception
#pragma align (exception)
struct exception {
  int     type;                         /* Function exception type          */
  char   *name;                         /* Name of function reporting error */
  double  arg1;                         /* First argument                   */
  double  arg2;                         /* Second argument (if present)     */
  double  retval;                       /* Value returned by function       */
};

#define _exception
#define _EXCEPTION_DEFINED
#endif

#ifndef __exceptionl
#pragma align (_exceptionl)
struct _exceptionl {
  int          type;                    /* Function exception type          */
  char        *name;                    /* Name of function reporting error */
  long double  arg1;                    /* First argument                   */
  long double  arg2;                    /* Second argument (if present)     */
  long double  retval;                  /* Value returned by function       */
};

#define __exceptionl
#define _LD_EXCEPTION_DEFINED
#endif

/*
 * Function prototypes:
 */
double acos(double);
double asin(double);
double atan(double);
double atan2(double, double);
double cabs(struct complex);
double ceil(double);
double cos(double);
double cosh(double);
double exp(double);
double fabs(double);
double floor(double);
double fmod(double, double);
double frexp(double, int *);
double hypot(double, double);
double j0(double);
double j1(double);
double jn(int, double);
double ldexp(double, int);
double log(double);
double log10(double);
int    matherr(struct exception *);   /* Stub for the user-supplied function */
double modf(double, double *);
double pow(double, double);
double sin(double);
double sinh(double);
double sqrt(double);
double square(double);
double tan(double);
double tanh(double);
double y0(double);
double y1(double);
double yn(int, double);

long double  acosl(long double);
long double  asinl(long double);
long double  atanl(long double);
long double  atan2l(long double, long double);
long double  cabsl(struct _complexl);
long double  ceill(long double);
long double  cosl(long double);
long double  coshl(long double);
long double  expl(long double);
long double  fabsl(long double);
long double  floorl(long double);
long double  fmodl(long double, long double);
long double  frexpl(long double, int *);
long double  hypotl(long double, long double);
long double _j0l(long double);
long double _j1l(long double);
long double _jnl(int, long double);
long double  ldexpl(long double, int);
long double  logl(long double);
long double  log10l(long double);
int         _matherrl(struct _exceptionl *);
long double  modfl(long double, long double *);
long double  powl(long double, long double);
long double  sinl(long double);
long double  sinhl(long double);
long double  sqrtl(long double);
long double  tanl(long double);
long double  tanhl(long double);
long double _y0l(long double);
long double _y1l(long double);
long double _ynl(int, long double);

/*
 * Prototypes of functions not supported by C Code Builder(TM) Libraries:
 */
int dieeetomsbin(double *, double *);
int dmsbintoieee(double *, double *);
int fieeetomsbin(float *, float *);
int fmsbintoieee(float *, float *);

/*
 * Map long double functions to the corresponding double functions
 * (long double is not a supported feature):
 */
#define  _complexl      complex                       /* for the cabsl macro */
#define  acosl(_a)      ( (long double) acos((double)(_a)) )
#define  asinl(_a)      ( (long double) asin((double)(_a)) )
#define  atanl(_a)      ( (long double) atan((double)(_a)) )
#define  atan2l(_a, _b) ( (long double) atan2((double)(_a), (double)(_b)) )
#define  cabsl(_a)      ( (long double) cabs(_a) )
#define  ceill(_a)      ( (long double) ceil((double)(_a)) )
#define  cosl(_a)       ( (long double) cos((double)(_a)) )
#define  coshl(_a)      ( (long double) cosh((double)(_a)) )
#define  expl(_a)       ( (long double) exp((double)(_a)) )
#define  fabsl(_a)      ( (long double) fabs((double)(_a)) )
#define  floorl(_a)     ( (long double) floor((double)(_a)) )
#define  fmodl(_a, _b)  ( (long double) fmod((double)(_a), (double)(_b)) )
#define  frexpl(_a, _b) ( (long double) frexp((double)(_a), _b) )
#define  hypotl(_a, _b) ( (long double) hypot((double)(_a), (double)(_b)) )
#define _j0l(_a)        ( (long double) j0((double)(_a)) )
#define _j1l(_a)        ( (long double) j1((double)(_a)) )
#define _jnl(_a, _b)    ( (long double) jn(_a, (double)(_b)) )
#define  ldexpl(_a, _b) ( (long double) ldexp((double)(_a), _b) )
#define  logl(_a)       ( (long double) log((double)(_a)) )
#define  log10l(_a)     ( (long double) log10((double)(_a)) )
#define _matherrl(_a)   matherr((struct exception *)(_a))
#define  modfl(_a, _b)  ( (long double) modf((double)(_a), (double *)(_b)) )
#define  powl(_a, _b)   ( (long double) pow((double)(_a), (double)(_b)) )
#define  sinl(_a)       ( (long double) sin((double)(_a)) )
#define  sinhl(_a)      ( (long double) sinh((double)(_a)) )
#define  sqrtl(_a)      ( (long double) sqrt((double)(_a)) )
#define  tanl(_a)       ( (long double) tan((double)(_a)) )
#define  tanhl(_a)      ( (long double) tanh((double)(_a)) )
#define _y0l(_a)        ( (long double) y0((double)(_a)) )
#define _y1l(_a)        ( (long double) y1((double)(_a)) )
#define _ynl(_a, _b )   ( (long double) yn(_a, (double)(_b)) )

/*
 * ANSI functions also prototyped in stdlib.h
 * for Microsoft(reg.) compatibility:
 */
#ifndef _math_stdlib
#define _math_stdlib
int           abs(int);
double        atof(const char *);
long double  _atold(const char *);
long          labs(long);
#define _atold(_a)     ( (long double) atof(_a) )
#endif

/*
 *  Compiler built-in functions:
 */
#ifndef _mathh_builtin
#define _mathh_builtin

#pragma _builtin_("_fabs_"==53)
double _fabs_(double);
#define fabs(x) _fabs_(x)

#pragma _builtin_("_sqrt_"==54)
double _sqrt_(double);
#define sqrt(x) _sqrt_(x)

#pragma _builtin_("_log_"==55)
double _log_(double);
#define log(x) _log_(x)

#pragma _builtin_("_log10_"==56)
double _log10_(double);
#define log10(x) _log10_(x)

#pragma _builtin_("_cos_"==57)
double _cos_(double);
#define cos(x) _cos_(x)

#pragma _builtin_("_sin_"==58)
double _sin_(double);
#define sin(x) _sin_(x)

#pragma _builtin_("_tan_"==59)
double _tan_(double);
#define tan(x) _tan_(x)

#pragma _builtin_("_acos_"==60)
double _acos_(double);
#define acos(x) _acos_(x)

#pragma _builtin_("_asin_"==61)
double _asin_(double);
#define asin(x) _asin_(x)

#pragma _builtin_("_atan2_"==62)
double _atan2_(double,double);
#define atan2(x,y) _atan2_(x,y)

#pragma _builtin_("_atan_"==63)
double _atan_(double);
#define atan(x) _atan_(x)

#pragma _builtin_("_exp_"==80)
double _exp_(double);
#define exp(x) _exp_(x)

#endif /* _mathh_builtin */

#endif /* _mathh */
