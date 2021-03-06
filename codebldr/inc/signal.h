/* signal.h - signal handling header file
 * $Version: 1.9 $
 * Copyright 1988,89,90,91 Intel Corporation, ALL RIGHTS RESERVED.
 */

#ifndef _signalh
#define _signalh

#ifndef _SIG_ATOMIC_T_DEFINED
typedef  char sig_atomic_t;
#define _SIG_ATOMIC_T_DEFINED
#endif

/*
 * Signal definitions:
 */
#define SIGILL    1                     /* Illegal instruction signal        */
#define SIGINT    2                     /* Interactive attention signal      */
#define SIGALLOC  3                     /* Memory allocation failure signal  */
#define SIGFREE   4                     /* Bad free pointer signal           */
#define SIGTERM   5                     /* Terminate signal                  */
#define SIGREAD   6                     /* Read error signal                 */
#define SIGWRITE  7                     /* Write error signal                */
#define SIGFPE    8                     /* Floating-point exception signal   */
#define SIGSEGV   9                     /* Segment violation signal          */
#define SIGABRT  10                     /* Abnormal termination signal       */
#define SIGBREAK 11                     /* Ctrl_break sequence               */
#define SIGUSR1  12                     /* User-defined signal 1             */
#define SIGUSR2  13                     /* User-defined signal 2             */
#define SIGUSR3  14                     /* User-defined signal 3             */

#define SIGSIZE  15                     /* Number of defined signals         */
#define NSIG     15                     /* Number of defined signals         */

extern void _sig_ill_dfl(void);
extern void _sig_int_dfl(void);
extern void _sig_alloc_dfl(void);
extern void _sig_free_dfl(void);
extern void _sig_term_dfl(void);
extern void _sig_read_dfl(void);
extern void _sig_write_dfl(void);
extern void _sig_fpe_dfl(int, int);
extern void _sig_segv_dfl(void);
extern void _sig_abrt_dfl(void);
extern void _sig_break_dfl(void);
extern void _sig_null(void);
extern void _sig_err_dummy(int);
extern void _sig_dfl_dummy(int);
extern void _sig_ign_dummy(int);
extern void _sig_sge_dummy(int);
extern void _sig_ack_dummy(int);

/*
 * Signal vector arrays:
 */
extern void (*_sig_eval[SIGSIZE])();
extern void (*_sig_dfl[SIGSIZE])();

/*
 * Signal processing macros:
 */
#define  SIG_IGN  (&_sig_ign_dummy)
#define  SIG_DFL  (&_sig_dfl_dummy)
#define  SIG_ERR  (&_sig_err_dummy)
#define  SIG_SGE  (&_sig_sge_dummy)
#define  SIG_ACK  (&_sig_ack_dummy)

/*
 * Function prototypes:
 */
void (*signal (int, void (*)(int)))(int);
int raise(int);

#endif /* _signalh */
