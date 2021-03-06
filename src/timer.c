/* @(#) timer.c Version 2.42 (6/26/92). copyright venue & Fuji Xerox  */
static char *id = "@(#) timer.c	2.42 6/26/92	(venue & Fuji Xerox)";

/************************************************************************/
/*									*/
/*				t i m e r . c				*/
/*									*/
/*	Timer handling routines, plus set-up for the other interrupts	*/
/*	Medley uses on Unix.						*/
/*									*/
/************************************************************************/


/************************************************************************/
/*									*/
/*	(C) Copyright 1989-92 Venue. All Rights Reserved.		*/
/*	Manufactured in the United States of America.			*/
/*									*/
/*	The contents of this file are proprietary information 		*/
/*	belonging to Venue, and are provided to you under license.	*/
/*	They may not be further distributed or disclosed to third	*/
/*	parties without the specific permission of Venue.		*/
/*									*/
/************************************************************************/



#ifdef DOS
#include <time.h>
#include <dos.h>
#include <i32.h>                       /* "#pragma interrupt" & '_chain_intr'*/
#include <dos.h>                       /* defines REGS & other structs       */
#include <stdio.h>                     /* define NULL                        */
#include <stdlib.h>
#define SIGVTALRM SIGUSR1
#define SIGIO SIGREAD
/******************************************************************************
*   Global variables
******************************************************************************/
void   (*prev_int_1c)();	/* keeps address of previous 1c handlr*/
				/* used for chaining & restore at exit*/
#pragma interrupt(DOStimer)
void	DOStimer();
unsigned long tick_count = 0;	/* approx 18 ticks per sec            */
#else
#include <sys/time.h>
#endif /* DOS */

#include	<sys/types.h>
#include	<stdio.h>
#include	<signal.h>
#include	<fcntl.h>

#ifdef ISC
#include	<sys/bsdtypes.h>
#include	<stropts.h>
#include	<sys/ioctl.h>
#include	<sys/times.h>
#define SIGIO SIGPOLL
extern long timezone;	/* seconds difference GMT to local */
#else
#ifndef DOS
#include        <sys/resource.h>
#endif /* DOS */
#endif /* ISC */
#include	<setjmp.h>

#ifdef AIXPS2
#include	<sys/fpcontrol.h>
#endif /* AIXPS2 */

	/* set up to use time() call if it's available */
#ifdef ISC
#define USETIMECALL
#elif DOS
#define USETIMECALL
#else
#undef USETIMECALL
#endif /* DOS or ISC 386 */



/* Apollo and Sun have different ideas about the name of this field */
#ifdef APOLLO
#define sv_flags sv_onstack
#endif

#include	"lispemul.h"
#include	"emlglob.h"
#include	"lspglob.h"
#include	"adr68k.h"
#include	"lsptypes.h"
#include	"arith.h"
#include	"lispmap.h"
#include	"stack.h"
#include	"dbprint.h"

#define	LISP_UNIX_TIME_DIFF	29969152
#define	LISP_ALTO_TIME_MASK	0x80000000
#define UNIX_ALTO_TIME_DIFF	2177452800
/*	Interlisp time is signed; MIN.FIXP = "01-JAN-01 00:00:00 GMT"
 *	Interlisp 0 is at	"19-Jan-69 12:14:08 PST"
 *	Unix begins at		" 1-Jan-70  0:00:00 GMT"
 *	(CL:- (IL:IDATE	" 1-Jan-70  0:00:00 GMT")
 *	      (IL:IDATE "19-Jan-69 12:14:08 PST"))
 *	=> 29969152, amount to add to Lisp time to get Unix time
 *	Alto time is unsigned; 0 = "01-JAN-01 00:00:00 GMT"
 *      UNIX_ALTO_TIME_DIFF is amount to add to Unix time
 *	to get Alto time.
 */

int	TIMEOUT_TIME;	/* For file system timeout */
char	*getenv();

#ifdef XWINDOW
#define FALSE 0
#define TRUE  !FALSE
int Event_Req = FALSE;
#endif /* XWINDOW */

/************************************************************************/
/*									*/
/*			i n i t _ m i s c s t a t s			*/
/*									*/
/*	Called at initialization time to set miscstats words.		*/
/*	?? and to periodically update them ?? [JDS 11/22/89]		*/
/*									*/
/*									*/
/************************************************************************/

init_miscstats()
  {
    MiscStats->starttime = gettime(0);
    MiscStats->gctime = 0;
    update_miscstats();
  }



/************************************************************************/
/*									*/
/*		    u p d a t e _ m i s c s t a t s			*/
/*									*/
/*	Updates counters and timers in the MISCSTATS "page".		*/
/*									*/
/*									*/
/*									*/
/************************************************************************/

update_miscstats()
  {
#ifdef DOS
    struct dostime_t dtm;	/* holds DOS time, so we can get .01 secs */
    _dos_gettime(&dtm);

    MiscStats->totaltime = (time(0)*1000) + (10 * dtm.hsecond);
    MiscStats->swapwaittime = 0;
    MiscStats->pagefaults = 0;  /* can't tell this on ISC */
    MiscStats->swapwrites = 0;
    MiscStats->diskiotime = 0; /* ?? not available ?? */
    MiscStats->diskops	= 0;
    MiscStats->secondstmp =
    	MiscStats->secondsclock = (time(0) + UNIX_ALTO_TIME_DIFF);
#elif !defined(ISC)
    struct timeval timev;
    struct rusage ru;

    getrusage(RUSAGE_SELF, &ru);

    MiscStats->totaltime = ru.ru_utime.tv_sec * 1000
			 + ru.ru_utime.tv_usec /1000;
    MiscStats->swapwaittime = ru.ru_stime.tv_sec * 1000
			    + ru.ru_stime.tv_usec /1000;
    MiscStats->pagefaults = ru.ru_minflt + ru.ru_majflt;
    MiscStats->swapwrites = ru.ru_majflt;
    MiscStats->diskiotime = 0; /* ?? not available ?? */
    MiscStats->diskops	= ru.ru_inblock
    			 /* ?? this doesn't work ???
    			+ ru.ru_outblock   */
				;
    gettimeofday(&timev, NULL);
    MiscStats->secondstmp =
    MiscStats->secondsclock = (timev.tv_sec + UNIX_ALTO_TIME_DIFF);
#else
    struct tms ru;

    times(&ru);	/* Get system time used */

    MiscStats->totaltime = ru.tms_utime*10 + ru.tms_stime*10
			 + ru.tms_cutime*10 + ru.tms_cstime*10;
  MiscStats->swapwaittime = ru.tms_stime*10
	    + ru.tms_cstime*10;
    MiscStats->pagefaults = 0;  /* can't tell this on ISC */
    MiscStats->swapwrites = 0;
    MiscStats->diskiotime = 0; /* ?? not available ?? */
    MiscStats->diskops	= 0;
    MiscStats->secondstmp =
    	MiscStats->secondsclock = (time(0) + UNIX_ALTO_TIME_DIFF);
  
#endif /* ISC */
  }



/************************************************************************/
/*									*/
/*			s u b r _ g e t t i m e				*/
/*									*/
/*	Handler for Lisps GETTIME subr call, dispatched thru		*/
/*	subr.c/miscn.c sub-dispatch.					*/
/*									*/
/*	Calls gettime, and returns the result to Lisp as a SMALLP	*/
/*	or FIXP, as appropriate.					*/
/*									*/
/************************************************************************/

DLword	*createcell68k();

LispPTR subr_gettime(args)
  LispPTR	args[];
  {
    int result;
    result = gettime(args[0] & 0xffff);
    if (args[1])
      {
	*((int*)Addr68k_from_LADDR(args[1]) ) = result;
	return (args[1]);
      }
    else N_ARITH_SWITCH(result);
}




/************************************************************************/
/*									*/
/*		    		g e t t i m e				*/
/*									*/
/*	Get the value of one of the various time counters, as		*/
/*	specified by the argument casep.  casep's values & meanings:	*/
/*									*/
/*	0 elapsed time, in milliseconds.				*/
/*	1 start of elapsed-time period, in milliseconds			*/
/*	2 this process's run time, in milliseconds			*/
/*	3 total GC time, in milliseconds				*/
/*	4 current time-of-day, in ALTO format				*/
/*	5 current time-of-day, in Interlisp format			*/
/*	6 start of daylight-savings, as day-in-year			*/
/*	7 end of daylight-savings, as day-in-year			*/
/*	8 time zone, as hours of offset from GMT (whole hours only)	*/
/*									*/
/************************************************************************/

int gettime(casep)
    int casep;
  {
#ifndef USETIMECALL
    struct	timeval	timev;
    struct	timezone tz;
#elif DOS
    struct dostime_t dtm;	/* for hundredths of secs */
#endif /* USETIMECALL */
    switch (casep)
      {

	case 0: /* elapsed time in alto milliseconds */
#ifdef USETIMECALL
#ifdef DOS
		_dos_gettime(&dtm);
		return ((time(0)+UNIX_ALTO_TIME_DIFF)*1000) + (10 * dtm.hsecond);
#else
		return ((time(0)+UNIX_ALTO_TIME_DIFF))*1000;
#endif /* DOS */
#else
		gettimeofday(&timev, NULL);
		return((timev.tv_sec + UNIX_ALTO_TIME_DIFF)*1000
				    + timev.tv_usec/1000);
#endif /* USETIMECALL */

	case 1: /* starting elapsed time in milliseconds */
		return(MiscStats->starttime);

	case 2: /* run time, this process, in milliseconds */
		update_miscstats();
		return(MiscStats->totaltime);

	case 3: /* total GC time in milliseconds */
		return(MiscStats->gctime);

	case 4: /* current time of day in Alto format */
#ifdef USETIMECALL
	    return(time(0) + UNIX_ALTO_TIME_DIFF);
#else
		gettimeofday(&timev, NULL);
		return (timev.tv_sec + UNIX_ALTO_TIME_DIFF);
#endif

	case 5:	/* current time of day in Interlisp format */
#ifdef USETIMECALL
	    return(time(0) + LISP_UNIX_TIME_DIFF);
#else
		gettimeofday(&timev, NULL);
		return(timev.tv_sec + LISP_UNIX_TIME_DIFF);
#endif

	case 6: 
		return (98); /* this is wrong, only works in PST */

	case 7:
		return (305); /* this is wrong, only works in PST */

	case 8:
#ifndef USETIMECALL
              gettimeofday(&timev, &tz);
		return (tz.tz_minuteswest/60); /* only integral timezones work */
#else
		return (timezone/3600);	/* timezone, extern, is #secs diff GMT to local. */
#endif

	default: return (0);
      }
  }



/************************************************************************/
/*									*/
/*			s u b r _ s e t t i m e				*/
/*									*/
/*	Converts its argument, a time in ALTO seconds, to the		*/
/*	UNIX time format, and sets the UNIX clock.  You must be		*/
/*	the super-user for this to work.				*/
/*									*/
/*	Implements the SETTIME subr call, sub-dispatched from subr.c	*/
/*									*/
/************************************************************************/

subr_settime(args)
  LispPTR	args[];
  {
#ifdef DOS
    struct dostime_t dostime;
    struct dosdate_t dosday;
    struct tm uxtime;

    uxtime = *localtime((time_t *)(*((int*)Addr68k_from_LADDR(args[0]))
				 - UNIX_ALTO_TIME_DIFF));
    dostime.hsecond = 0;
    dostime.second = uxtime.tm_sec;
    dostime.minute = uxtime.tm_min;
    dostime.hour = uxtime.tm_hour;
    _dos_settime(&dostime);

    dosday.day = uxtime.tm_mday;
    dosday.month = uxtime.tm_mon;
    dosday.year = uxtime.tm_year;
    dosday.dayofweek = uxtime.tm_wday;
    _dos_setdate(&dosday);

#elif SYSVONLY
    stime(*((int*)Addr68k_from_LADDR(args[0]))
                - UNIX_ALTO_TIME_DIFF);
#else
    struct timeval timev;
    timev.tv_sec = *((int*)Addr68k_from_LADDR(args[0]))
    		- UNIX_ALTO_TIME_DIFF;
    settimeofday(&timev, NULL);
#endif /* DOS */
  } /* end subr_settime */





/************************************************************************/
/*									*/
/*		    s u b r _ c o p y t i m e s t a t s			*/
/*									*/
/*	Given source and destination MISCSTATS structure pointers,	*/
/*	copy the contents of the source structure into the dest.	*/
/*									*/
/*	Also calls update_miscstats, to keep stats current.		*/
/*									*/
/************************************************************************/

subr_copytimestats(args)
  LispPTR args[];
 {
    MISCSTATS *source;
    MISCSTATS *dest;
    source = (MISCSTATS*) Addr68k_from_LADDR(args[0]);
    dest   = (MISCSTATS*) Addr68k_from_LADDR(args[1]);
    update_miscstats();
    *dest = *source;
  }



/************************************************************************/
/*									*/
/*			     N _ O P _ r c l k				*/
/*									*/
/*	Get the current time in UNIX format, convert it to micro-	*/
/*	seconds in ALTO format, and store the low 32 bits into		*/
/*	the FIXP cell passed in to us on the top of stack.		*/
/*									*/
/************************************************************************/

LispPTR N_OP_rclk(tos)
  register LispPTR tos;
  {
    unsigned int	usec;
#ifdef DOS
    struct dostime_t dtm;
#endif /* DOS */

#ifdef USETIMECALL
#ifdef DOS
    _dos_gettime(&dtm);
    usec = (time(0)*1000000) + (10000 * dtm.hsecond);
#else
    usec = time(0)*1000000;
#endif /* DOS */
#else
    struct timeval timev;

    gettimeofday(&timev, NULL);
    usec = (timev.tv_sec * 1000000) + timev.tv_usec;
#endif /* USETIMECALL */
    *((unsigned int *)(Addr68k_from_LADDR(tos))) = usec;
    return(tos);
  } /* end N_OP_rclk */



/**********************************************************************/
 /* update_timer called periodically */

/************************************************************************/
/*									*/
/*									*/
/*									*/
/*									*/
/*									*/
/*									*/
/*									*/
/*									*/
/************************************************************************/

update_timer()
  {
#ifdef USETIMECALL
    MiscStats -> secondstmp = MiscStats->secondsclock
			= time(0) + UNIX_ALTO_TIME_DIFF;
#else
    struct  timeval timev;
    gettimeofday(&timev, NIL);
    MiscStats->secondstmp = MiscStats->secondsclock
			  = (timev.tv_sec + UNIX_ALTO_TIME_DIFF);
#endif /* USETIMECALL */
  }


/************************************************************************/
/*									*/
/*									*/
/*									*/
/*									*/
/*									*/
/*									*/
/*									*/
/*									*/
/************************************************************************/


/**********************************************************************/
/* timer interrupt handling system

	int_init() should be called before first entering dispatch loop.
	int_timer_init() is called by int_init() and arms the timer interrupt.
	int_io_init() is called by int_init() and arms the I/O interrupt.
	int_timer_service() catches the timer signal and sets
		Irq_Stk_Check & Irq_Stk_End to 0
		so the rest of the system will see it and respond.
	int_block() and int_unblock() block timer interrupts  and release them.
	int_io_open(fd) should be called whenever a file that should interrupt
		us is opened; it enables the interrupt on that fd.
	int_io_close(fd) should be called whenever a file that should interrupt
		us is closed; it disables the interrupt on that fd.
*/

/* TIMER_INTERVAL usec ~ 20  per second.  This should live in some
	machine-configuration
	file somewhere - it can be changed as the -t parameter to lisp*/

#ifdef sparc
int TIMER_INTERVAL =  100000;
#else
int TIMER_INTERVAL =  25000;
#endif

int FileIOFlag = 0;
int TimerFlag = 0;
extern u_int LispWindowFd;

/************************************************************************/
/*									*/
/*		    i n t _ t i m e r _ s e r v i c e			*/
/*									*/
/*	Handle the virtual-time alarm signal VTALRM.  If running in	*/
/*	HPUX, re-set the alarm ourselves, because the OS walks on	*/
/*	your timer if you let IT do the resetting.			*/
/*									*/
/*									*/
/************************************************************************/

#ifndef SYSVSIGNALS
static struct sigvec timerv;
#endif /* SYSVSIGNALS */

/* #if (defined(OS4) || defined(SYSVONLY)) */
void int_timer_service(sig, code, scp)
/* #else
int int_timer_service(sig, code, scp)
#endif OS4 | SYSVONLY */

  int sig, code;
  struct sigcontext *scp;
  {
/* this may have to do more in the future, like check for nested interrupts,
	etc... */

    Irq_Stk_Check = 0;
    Irq_Stk_End = 0;
    TimerFlag = 1;

#ifdef XWINDOW
    Event_Req = TRUE;
#endif
#ifdef HPTIMERBUG
      {
struct itimerval timert, tmpt;
	timert.it_interval.tv_sec  = timert.it_value.tv_sec = 0;
	timert.it_interval.tv_usec = 0;
	timert.it_value.tv_usec = TIMER_INTERVAL;
	setitimer(ITIMER_VIRTUAL, &timert, 0);
      }
#endif /* HPTIMERBUG */

#ifdef SYSVSIGNALS
#ifndef ISC
    signal(SIGVTALRM, int_timer_service);
#endif /* ISC */
#endif /* SYSVSIGNALS */
}




/************************************************************************/
/*									*/
/*									*/
/*									*/
/*									*/
/*									*/
/*									*/
/*									*/
/*									*/
/************************************************************************/

void int_timer_init()
  {
#ifdef DOS
/******************************************************************************
*  All code and data touched during the processing of an interrupt should
*  locked prior to receiving any interrupts.  This prevents the Timer
*  function from being swapped out during an interrupt.
******************************************************************************/
   _dpmi_lockregion((void *)TimerFlag, sizeof(TimerFlag));
   _dpmi_lockregion((void *)Irq_Stk_End, sizeof(Irq_Stk_End));
   _dpmi_lockregion((void *)Irq_Stk_Check, sizeof(Irq_Stk_Check));
   _dpmi_lockregion((void *)tick_count, sizeof(tick_count));
   _dpmi_lockregion((void *)&DOStimer, 4096);
   
				/* Set up the DOS time handler. */
    prev_int_1c = _dos_getvect(0x1c); /* get addr of currnt 1c hndlr, */
				      /* if any*/ 
    _dos_setvect(0x1c, DOStimer);	/* hook our int handler to timer int */


#else /* DOS */
#ifndef ISC
    struct itimerval timert, tmpt;

#ifdef SYSVSIGNALS
    signal(SIGVTALRM, int_timer_service);
#else
    /* first set up the signal handler */
    timerv.sv_handler = int_timer_service;
    timerv.sv_mask = timerv.sv_flags = 0;
    sigvec(SIGVTALRM, &timerv, 0);
#endif /* SYSVSIGNALS */

#ifdef HPTIMERBUG
    /* HPUX on the series 700 trashes the timer if you use */
    /* the auto-reset feature (interval != 0), so have to  */
    /* move the reset into the timer handler (above).      */
    timert.it_interval.tv_sec  = timert.it_value.tv_sec = 0;
    timert.it_interval.tv_usec = 0;
    timert.it_value.tv_usec = TIMER_INTERVAL;
#else
    timert.it_interval.tv_sec  = timert.it_value.tv_sec = 0;
    timert.it_interval.tv_usec = timert.it_value.tv_usec = TIMER_INTERVAL;
#endif /* HPTIMERBUG */


    /* then attach a timer to it and turn it loose */
    timerclear(&tmpt.it_value);
    timerclear(&tmpt.it_interval);
    setitimer(ITIMER_VIRTUAL, &timert, &tmpt);
    getitimer(ITIMER_VIRTUAL, &tmpt);

    DBPRINT(("Timer interval set to %d usec\n", timert.it_value.tv_usec));
#endif /* ISC */
#endif /* DOS */
}



/************************************************************************/
/*									*/
/*									*/
/*									*/
/*									*/
/*									*/
/*									*/
/*									*/
/*									*/
/************************************************************************/


void int_io_open(fd)
  int fd;
  {
#ifdef DOS
    /* would turn on DOS kbd signal handler here */
#elif KBINT

    DBPRINT(("int_io_opening %d\n", fd));
    if(fcntl(fd, F_SETOWN, getpid()) == -1 ) {
#ifdef DEBUG
	perror("fcntl F_SETOWN ERROR");
#endif
		};
    if(fcntl(fd, F_SETFL, fcntl (fd, F_GETFL, 0) | FASYNC) == -1)
	perror("fcntl F_SETFL error");
#endif
}

void int_io_close(fd)
  int fd;
  {
#ifdef DOS
    /* Turn off signaller here */
#elif  KBINT
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) & ~FASYNC);
#endif
  }


/************************************************************************/
/*									*/
/*			i n t _ i o _ i n i t				*/
/*									*/
/*	Set up handling for the SIGIO and SIGPOLL signals, in		*/
/*	support of keyboard event handling and ethernet incoming-	*/
/*	packet handling.						*/
/*									*/
/*									*/
/************************************************************************/

void int_io_init()
  {
#ifndef SYSVSIGNALS
    static struct sigvec timerv;
    static struct sigvec poll_timerv;
#endif /* SYSVSIGNALS */
    extern void getsignaldata();

    /* first set up the signal handler */

#ifndef SYSVSIGNALS
#ifdef KBINT
    timerv.sv_handler = getsignaldata;
    timerv.sv_mask = timerv.sv_flags = 0;
    sigvec(SIGIO, &timerv, 0);
 
    DBPRINT(("I/O interrupts enabled\n"));
#endif /* KBINT */
#else /* SYSVSIGNALS in effect... */
#ifdef ISC
	{
	  int res = sigset(SIGIO, getsignaldata);
	  if (res==SIG_ERR) perror("sigset for I/O polling");
	  if (ioctl(ConnectionNumber(currentdsp->display_id)
		    , I_SETSIG, S_INPUT) < 0) perror("ioctl on Xfd SETSIG");
	}
#else
    signal(SIGIO, getsignaldata);
#endif /* ISC */
#endif /* SYSVSIGNALS */

}

int oldmask = 0;


/************************************************************************/
/*									*/
/*			   i n t _ b l o c k				*/
/*									*/
/*	Temporarily turn off interrupts.				*/
/*									*/
/*	NOTE that these interrupts must also be turned off in ldeboot's	*/
/*	forking code; if you change these, go fix that one too		*/
/*									*/
/************************************************************************/

void int_block()
{
/* temporarily turn off interrupts */
#ifdef DOS
  _dos_setvect(0x1c, prev_int_1c);
#else /* DOS */
#ifdef SYSVSIGNALS
#ifndef ISC
    sighold(SIGVTALRM);
#endif /* ISC */
    sighold(SIGIO);
#ifdef SIGALRM
    sighold(SIGALRM);
#endif /* SIGALRM */
#ifdef SIGXFSZ
    sighold(SIGXFSZ);
#endif /* SIGXFSZ */
#else
	oldmask= sigblock(sigmask(SIGVTALRM) | sigmask(SIGIO)
			  | sigmask(SIGALRM) 
#ifndef HPUX
			  | sigmask(SIGXFSZ)
#endif /* HPUX */
#ifdef FLTINT
 			  | sigmask(SIGFPE)
#endif
);
#endif /* SYSVSIGNALS */
#endif /* DOS */
}


/************************************************************************/
/*									*/
/*									*/
/*									*/
/*									*/
/*									*/
/*									*/
/*									*/
/*									*/
/************************************************************************/

void int_unblock()
{
#ifdef DOS
    _dos_setvect(0x1c, DOStimer);
#else /* DOS */
#ifdef SYSVSIGNALS
#ifndef ISC
    sigrelse(SIGVTALRM);
#endif /* ISC */
    sigrelse(SIGIO);
#ifdef SIGALRM
    sigrelse(SIGALRM);
#endif /* SIGALRM */
#ifdef SIGXFSZ
    sigrelse(SIGXFSZ);
#endif /* SIGXFSZ */
#else
	sigsetmask(oldmask);
#endif /* SYSVSIGNALS */
#endif /* DOS */
}

int_timer_on () {int_unblock();}
int_timer_off() {int_block  ();}


/************************************************************************/
/*									*/
/*									*/
/*									*/
/*									*/
/*									*/
/*									*/
/*									*/
/*									*/
/************************************************************************/


/* * * * * * * dyked out in favor of calling int_block * * * * *
void int_timer_off()
  {
#ifdef SYSVSIGNALS
#ifndef ISC
    sigignore(SIGVTALRM);
#endif ISC
#else
    struct sigvec tmpv, timeroffv;

    timeroffv.sv_handler = SIG_IGN;
    timeroffv.sv_mask = timeroffv.sv_flags = 0;
    sigvec(SIGVTALRM, &timeroffv, &tmpv);
#endif SYSVSIGNALS
}

void int_timer_on()
{
#ifdef SYSVSIGNALS
#ifndef ISC
    signal(SIGVTALRM, int_timer_service);
#endif ISC
#else
    struct sigvec tmpv;

    sigvec(SIGVTALRM, &timerv, &tmpv);
#endif SYSVSIGNALS

}
* * * * * * * * * */



#ifdef FLTINT
/************************************************************************/
/*									*/
/*  F L O A T I N G - P O I N T   I N T E R R U P T   H A N D L I N G	*/
/*									*/
/*	This is the handler for the SIGFPE signal, to catch floating-	*/
/*	point exceptions.  Sets the global 'FP_error' to the error	*/
/*	code passed in by the signal; FP_error is checked by the	*/
/*	Lisp emulator FP code to make sure everything is OK.		*/
/*									*/
/************************************************************************/

/* The global used to signal floating-point errors */
int FP_error = 0;

void int_fp_service(sig, code, scp)
  int sig, code;
  struct sigcontext *scp;
  {
    switch(code)
      {
#ifdef AIXPS2
	case FPM_DENORM:
	case FPM_DIVIDE_0:
	case FPM_UNDERFLOW:
	case FPM_OVERFLOW:
	case FPM_PRECISION:
#else
#ifndef ISC
	case FPE_FLTDIV_TRAP:
	case FPE_FLTUND_TRAP:
	case FPE_FLTOVF_TRAP:
	case FPE_FLTOPERR_TRAP:
#endif /* ISC */
#endif /* AIXPS2 */
				FP_error = code;
				break;
	default:
	  {
#ifdef DEBUG
	    char stuff[100];
	    sprintf(stuff, "Unexpected FP error signal code: %d", code);
	    perror(stuff);
#endif
	  }
      }
  }



int_fp_init()
  {	/* first set up the signal handler */
#ifndef ISC
#ifndef AIXPS2
    if(ieee_handler("set", "all", int_fp_service))
#else
    if(signal(SIGFPE, int_fp_service))
#endif /* AIXPS2 */
      perror("Sigvec for FPE failed");
    DBPRINT(("FP interrupts enabled\n"));
#endif /* ISC */
  }

#endif /* FLTINT */




/************************************************************************/
/*									*/
/*			t i m e o u t _ e r r o r			*/
/*									*/
/*	Error handling routine for SIGALRM.  Called when any		*/
/*	TIMEOUT(...) forms spend more than TIMEOUT_TIME (normally	*/
/*	10 sec.) trying to do an I/O operation.				*/
/*									*/
/*									*/
/************************************************************************/

jmp_buf	jmpbuf;
void timeout_error()
  {
    /*
     * Following printf changes the contents of jmpbuf!
     * This would lead to horrible segmentation violation.
     */
/*  printf("File access timed out.\n"); */
#ifdef SYSVSIGNALS
#ifdef SIGALRM
	signal(SIGALRM, timeout_error);
#endif
#endif /* SYSVSIGNALS */
    longjmp(jmpbuf, 1);
  }



/************************************************************************/
/*									*/
/*			i n t _ f i l e _ i n i t			*/
/*									*/
/*	Set up the signal handler for SIGALRM, to catch TIMEOUTs:	*/
/*	TIMEOUT(...) forms spend more than TIMEOUT_TIME (normally	*/
/*	10 sec.) trying to do an I/O operation.				*/
/*									*/
/*									*/
/************************************************************************/

void int_file_init()
  {
#ifndef SYSVSIGNALS
    static struct sigvec timerv;
#endif /* SYSVSIGNALS */
    char	*envtime;
    int	timeout_time;

    /* first set up the signal handler */
#ifndef SYSVSIGNALS
    timerv.sv_handler = timeout_error;
    timerv.sv_mask = timerv.sv_flags = 0;
    sigvec(SIGALRM, &timerv, 0);
#else
#ifdef SIGALRM
#ifdef ISC
	sigset(SIGALRM, timeout_error);
#else
	signal(SIGALRM, timeout_error);
#endif /* ISC */
#endif /* SIGALRM */
#endif /* SYSVSIGNALS */

    /* Set Timeout period */
    if((envtime = getenv("LDEFILETIMEOUT")) == NULL)
      {
	TIMEOUT_TIME = 10;
      }
    else
      {
	if( (timeout_time = atoi(envtime)) > 0) TIMEOUT_TIME = timeout_time;
	else TIMEOUT_TIME = 10;
      }
    DBPRINT(("File timeout interrupts enabled\n"));
  }



/************************************************************************/
/*                                                                      */
/*                         p a n i c u r a i d                          */
/*                                                                      */
/*   Most of the unused process-killing interrupts end up here; you     */
/*      can't do a whole lot safely here but dump your sysout for       */
/*      post-mortem analysis, but you MIGHT be able to get a clue       */
/*      about what killed you.                                          */
/*                                                                      */
/************************************************************************/

void panicuraid(sig, code, scp, addr)
  int sig, code;
  struct sigcontext *scp;
  {
    static char errormsg[200];
    static char *stdmsg =
"Please record the signal and code information\n\
and do a 'v' before trying anything else.";
    int i;

    for (i=0; i<200; i++) errormsg[i]=0;

    switch (sig)
      {
#ifdef SIGBUS
	case SIGBUS:	sprintf(errormsg,
				"BUS error (code %d) at address 0x%x.\n%s",
				code, addr, stdmsg);
			break;
#endif /* SIGBUS */
	case SIGSEGV:	sprintf(errormsg,
				"SEGV error (code %d) at address 0x%x.\n%s",
				code, addr, stdmsg);
			break;
	case SIGILL:	sprintf(errormsg,
				"Illegal instruction (code %d) at address 0x%x.\n%s",
				code, addr, stdmsg);
			break;
#ifdef SIGPIPE
	case SIGPIPE:	sprintf(errormsg,
				"Broken PIPE (code %d) at address 0x%x.\n%s",
				code, addr, stdmsg);
			break;
#endif /* SIGPIPE */
#ifdef SIGHUP
	case SIGHUP:	sprintf(errormsg,
				"HANGUP signalled (code %d) at address 0x%x.\n%s",
				code, addr, stdmsg);
			/* Assume that a user tried to exit UNIX shell */
#ifdef SYSVONLY
			kill(0, SIGKILL); exit(0);
#else
			killpg(getpgrp(0), SIGKILL);exit(0);

#endif /* SYSVONLY */
			break;
#endif /* SIGHUP */
	case SIGFPE:	sprintf(errormsg,
				"FP error (code %d) at address 0x%x.\n%s",
				code, addr, stdmsg);
			break;
	default:	sprintf(errormsg, "Uncaught SIGNAL %d (code %d).\n%s",
				sig, code, stdmsg);
      }

    error(errormsg);

  }




/************************************************************************/
/*                                                                      */
/*                     i n t _ p a n i c _ i n i t                      */
/*                                                                      */
/*  A catch for all the deadly interupts (but KILL, of course)          */
/*  Dumps you into uraid; you probably can't get back from it,          */
/*     but there is hope that you will be able to poke around with      */
/*     uraid and get a clue about why you're dying.                     */
/*                                                                      */
/************************************************************************/
void int_panic_init()
  {
#ifdef DOS
	/* for now, nothing */
   /* if (signal(SIGFPE, panicuraid)) perror("SIGFPE hander setup failed");*/
    signal(SIGILL, panicuraid);
    signal(SIGSEGV, panicuraid);
#elif SYSVSIGNALS
#ifdef ISC
	sigset(SIGHUP, panicuraid);
	sigset(SIGQUIT, panicuraid);
	sigset(SIGILL, panicuraid);
	sigset(SIGEMT, panicuraid);
	sigset(SIGBUS, panicuraid);
	sigset(SIGSEGV, panicuraid);
	sigset(SIGSYS, panicuraid);
	sigset(SIGTERM, panicuraid);
#else	
	signal(SIGHUP, panicuraid);
    signal(SIGQUIT, panicuraid);
    signal(SIGILL, panicuraid);
    signal(SIGEMT, panicuraid);
    signal(SIGBUS, panicuraid);
    signal(SIGSEGV, panicuraid);
    signal(SIGSYS, panicuraid);
    signal(SIGTERM, panicuraid);
#ifndef FLTINT
    if (signal(SIGFPE, panicuraid)) perror("SIGFPE hander setup failed");
#endif /* FLTINT */
#endif /* ISC */
#else
    static struct sigvec panicv;
    static struct sigvec ignorev;

    /* first set up the signal handlers: */

    panicv.sv_handler = panicuraid;
    panicv.sv_mask = panicv.sv_flags = 0;
    ignorev.sv_handler = SIG_IGN;
    ignorev.sv_mask = ignorev.sv_flags = 0;

    /* Now arrange for signals to be handled properly: */

    sigvec(SIGHUP,  &panicv, 0);
/*  sigvec(SIGINT,  &panicv, 0); */
    sigvec(SIGQUIT, &panicv, 0);
    sigvec(SIGILL,  &panicv, 0);
/*  sigvec(SIGTRAP, &panicv, 0); */
#ifdef OS4
    sigvec(SIGABRT, &panicv, 0);
#endif /* OS4 */
    sigvec(SIGEMT,  &panicv, 0);
    sigvec(SIGBUS,  &panicv, 0);
    sigvec(SIGSEGV, &panicv, 0);
    sigvec(SIGSYS,  &panicv, 0);
/*  sigvec(SIGPIPE, &panicv, 0);  Caused trouble with TCP; now ignored: */
    sigvec(SIGPIPE, &ignorev, 0);
    sigvec(SIGTERM, &panicv, 0);
#ifdef OS4
    sigvec(SIGXCPU, &panicv, 0);
    sigvec(SIGLOST, &panicv, 0);
#endif /* OS4 */
    sigvec(SIGUSR1, &panicv, 0);
    sigvec(SIGUSR2, &panicv, 0);
#ifndef FLTINT
    sigvec(SIGFPE, &panicv, 0);
#endif
#endif /* SYSVSIGNALS  & DOS */

    DBPRINT(("Panic interrupts enabled\n"));
  }



/************************************************************************/
/*									*/
/*			    i n t _ i n i t				*/
/*									*/
/*	Initialize all the interrupts for Lisp & the emulator.		*/
/*									*/
/************************************************************************/

void int_init()
  {
   int_timer_init();	/* periodic interrupt timer */
   int_io_init();	/* SIGIO and SIGPOLL async I/O handlers */
   int_file_init();	/* file-io TIMEOUT support */
   int_panic_init();    /* catch for all other dangerous interrupts */

#ifdef FLTINT
   int_fp_init();	/* Floating-point exception handler */
#endif

   int_unblock();	/* Turn on interrupts */
}

#ifdef DOS
/******************************************************************************
*  DOStimer()
*
*  The interrupt 0x1c handler.  This routine must be declared using the
*  '#pragma interrupt()' statement to ensure that all registers are preserved.
*  It is also needed to ensure the proper functioning of '_chain_intr()'.
*
*  The timer interrupt (normally) occurs 18.2 times per second.  This routine
*  waits one extra tick every 91 ticks (18.2*5).
*
*  Before this interrupt was installed, 'prev_int_1c' was set to the current
*  0x1c interrupt. 'DOStimer()' chains to this interrupt using '_chain_intr()',
*  rather than returning back to the caller.
*
*  Note that as little as possible should be done within a timer interrupt,
*  since further clock ticks are disabled until the interrupt returns.
******************************************************************************/
void  DOStimer()
{
 /* if (--tick_count == 0) { */
    Irq_Stk_Check = 0;
    Irq_Stk_End = 0;
    TimerFlag = 1;
 /*   _dos_setvect(0x1c, prev_int_1c);
  } else if (tick_count <= 0) { */
    /* I'm dead, uninstal me */
 /*   _dos_setvect(0x1c, prev_int_1c);
    tick_count = 0;
  } */
  _chain_intr(prev_int_1c);	/* call previous int 1c handlr, if any*/
  /* (pts to 'ret' if no prev installed)*/
}

void alarm( sec )
     unsigned long sec;
{
  /* tick_count = sec * 18;
  _dos_setvect(0x1c, DOStimer); */
}
#endif /* DOS */

