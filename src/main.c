/* @(#) main.c Version 1.5 (6/7/91). copyright venue & Fuji Xerox  */
static char *id = "@(#) main.c	1.5 6/7/91	(venue & Fuji Xerox)";




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

/*
 *	main.c
 *	This file includes main()
 */

#include "dbprint.h"
#include	<sys/types.h>
#ifndef DOS
#include	<sys/param.h>
#include        <sys/time.h>
#else
#define MAXPATHLEN 128
#define R_OK 04
#define index strchr
#define rindex strrchr
#include <time.h>
#include <string.h>
#endif /* DOS */

#ifndef NOETHER
#include	<net/nit.h>	/* needed for Ethernet stuff below */
#endif /* NOETHER */

#include	<stdio.h>
#ifndef HPUX
#ifndef DOS
#include	<strings.h>
#endif /* DOS */
#endif /* HPUX */

#ifndef DOS
#include	<sys/file.h>
#endif /* DOS */

#include	<setjmp.h>
#ifndef DOS
#include	<pwd.h>
#endif /* DOS */

#ifdef RISCOS
#include <unistd.h>
#endif /* RISCOS */


#include	<errno.h>

#include "lispemul.h"
#include "emlglob.h"
#include "address.h"
#include "adr68k.h"
#include "stack.h"
#include "return.h"

#include "lspglob.h"
#include "lsptypes.h"
#include "lispmap.h"
#include "ifpage.h"
#include "iopage.h"

#include "debug.h"
#include "profile.h"

#include "timeout.h"

DLword	*Lisp_world;		/* lispworld */

/********** 68k address for Lisp Space **********/
DLword	*Atomspace;
DLword	*Stackspace;
DLword	*Plistspace;
DLword	*DTDspace;
DLword	*MDStypetbl;
DLword	*AtomHT;
DLword	*Pnamespace;
DLword	*Defspace;
DLword	*Valspace;
DLword	*Spospspace;
DLword	*Snegspace;

/********** For Virtual Memory Management **********/
DLword	*FPtoVP;
DLword	*PAGEMap;
DLword	*PageMapTBL;
DLword	*LockedPageTable;

/********** For Interface to LispMicro/Device **********/
DLword	*IOCBPage;
IOPAGE	*IOPage;
IFPAGE	*InterfacePage;
MISCSTATS	*MiscStats;

/********** UFN Table **********/
DLword	*UFNTable;

/********** Tables for GC **********/
DLword	*HTmain;
DLword	*HToverflow;
DLword	*HTbigcount;
DLword	*HTcoll;

/********** Display **********/
DLword	*DisplayRegion;
int DisplayInitialized = NIL;

DLword	*Arrayspace;
DLword	*MDS_space_bottom;
DLword	*PnCharspace;
struct dtd	*ListpDTD;


/********** For Lisp Emulator **********/
struct	state	MachineState;

/**********************************/
/*** Share val with LISP code ******/

DLword *MDS_free_page;
DLword *Next_MDSpage;
DLword *Next_Array;
/*******************************************/

/** CACHE LISP SYSVAL ***/
LispPTR *Next_MDSpage_word;
LispPTR *Next_Array_word;
LispPTR *MDS_free_page_word;

LispPTR *Reclaim_cnt_word ;

/*** Cache Values for reclaimer by Tomtom 30-Sep-1987 ***/
LispPTR *GcDisabled_word;
LispPTR *CdrCoding_word;
LispPTR *FreeBlockBuckets_word;
LispPTR *Array_Block_Checking_word;
LispPTR *ArrayMerging_word;
LispPTR *ArraySpace_word;
LispPTR *ArraySpace2_word;
LispPTR *ArrayFrLst_word;
LispPTR *ArrayFrLst2_word;
LispPTR *Hunk_word;
LispPTR *System_Buffer_List_word;

/*** The end of the addition of cache values on reclaimer ***/


/*** cache values for the top level reclaimer's implementation ***/

LispPTR *GcMess_word;
LispPTR *ReclaimMin_word;
LispPTR *GcTime1_word;
LispPTR *GcTime2_word;
LispPTR *MaxTypeNumber_word;

/*** The end of the addition of cache values for top reclaimer by Tomtom
						15-Oct-1987             ***/

/*  Pointers for closure caching */

LispPTR *Package_from_Index_word;
LispPTR *Package_from_Name_word;
LispPTR *Keyword_Package_word;
LispPTR *Closure_Cache_Enabled_word;
LispPTR *Closure_Cache_word;
LispPTR *Deleted_Implicit_Hash_Slot_word;
LispPTR First_index;

/*** The end of Pointers for closure caching ***/

/* CACHE values for 32Mb MDS/Array by Take */
LispPTR *STORAGEFULLSTATE_word;
LispPTR *STORAGEFULL_word;
LispPTR *PENDINGINTERRUPT_word;
LispPTR *LeastMDSPage_word;
LispPTR *SecondMDSPage_word;
LispPTR *SecondArrayPage_word;
LispPTR *INTERRUPTSTATE_word;
LispPTR *SYSTEMCACHEVARS_word;
LispPTR *MACHINETYPE_word;

LispPTR STORAGEFULLSTATE_index;
LispPTR *LASTVMEMFILEPAGE_word;
LispPTR *VMEM_FULL_STATE_word ;

/** Array for N-tran **/

int native_load_address;
LispPTR native_closure_env = NOBIND_PTR;

/** Pipes for Unix Interface **/
int UnixPipeIn;
int UnixPipeOut;
int UnixPID;
int please_fork = 1;

/*** STACK handle staff(Takeshi) **/
LispPTR *STACKOVERFLOW_word;
LispPTR *GuardStackAddr_word;
LispPTR *LastStackAddr_word;
LispPTR *NeedHardreturnCleanup_word;

/*** Ethernet stuff (JRB) **/
#ifndef NOETHER
extern int ether_fd;
extern u_char ether_host[6];
#endif /* NOETHER */

extern struct sockaddr_nit snit;

int	for_makeinit = 0;
int	kbd_for_makeinit = 0;
int	save_argc;
char	**save_argv;
int	display_max = 65536*16*2;

/** For call makepathname inside main() **/
extern int	*Lisp_errno;
extern int	Dummy_errno;		/* If errno cell is not provided by Lisp, dummy_errno is used. */

char keystring[128];

#define FALSE 0
#define TRUE  !FALSE

char *getenv();
int Lisp_Xinitialized = FALSE;
char sysout_name[1024];
int sysout_size = 0; /* default 0 */ /* was = 32;*/

extern int sysout_loader();

#if defined(DOS) || defined (XWINDOW)
#include "dspif.h"
extern DspInterface currentdsp;
#endif /* DOS || XWINDOW */

#ifdef DOS
int dosdisplaymode = 0;
#endif /* DOS */

/************************************************************************/
/*									*/
/*		     M A I N   E N T R Y   P O I N T			*/
/*									*/
/*									*/
/************************************************************************/

main(argc, argv)
  int	argc ;
  char	**argv ;
  {
    int i;
    char *envname;
    extern int TIMER_INTERVAL;
    char  keytyped [ 255 ];

#ifndef NOFORN
    if (dld_find_executable (argv[0]) == 0) {
      perror("Name of executable not found.");
    } else if (dld_init (dld_find_executable (argv[0])) != 0) {
      dld_perror( "Can't init DLD." );
    };
#endif /* NOFORN */

#ifdef XWINDOW
	read_Xoption( &argc, argv );
#endif /* XWINDOW */


    save_argc = argc;
    save_argv = argv;

#ifdef PROFILE
    moncontrol(0); /* initially stop sampling */
#endif /* PROFILE */


/* Sysout is found as follows:
	If the first argument doesn't begin with '-', assume it's the sysout
	Look at the environment variable LDESRCESYSOUT if that fails
	Look for ~/lisp.virtualmem if that fails
	Barf and print the command line if tha fails
*/

#ifdef	FSERROR
    /* For call makepathname */
    Lisp_errno = &Dummy_errno;
#endif

    i=1;
    if(argc > 1 && argv[1][0] != '-')
      {
	strcpy(sysout_name, argv[1]);
	i++;
      }
     else if ((envname = getenv("LDESRCESYSOUT")) != NULL)
      {
	strcpy(sysout_name, envname);
      }
    else if ((envname = getenv("LDESOURCESYSOUT")) != NULL)
      strcpy(sysout_name, envname);
#ifdef DOS
    else if( !makepathname("~/lisp.vm", sysout_name)
#else
    else if( !makepathname("~/lisp.virtualmem", sysout_name)
#endif /* DOS */
	    || access(sysout_name, R_OK))
      {
	fprintf(stderr, "Coudn't find a sysout to run;\n");
#ifdef DOS
	fprintf(stderr, "medley [sysout-name] [-m <memory in Mb>] [-vga]\n" );
#else
	fprintf(stderr, "either setenv LDESRCESYSOUT or do:\n");
	fprintf(stderr, "lde[ether] [sysout] [-k access-key] [-E <ethernet-info>]\n" );
#endif /* DOS */
	exit(1);
      }
/* OK, sysout name is now in sysout_name, and i is moved past a supplied name */

    for ( ; i < argc; i += 1 )
      { /* step by 1 in case of typo */
	/* -t and -m are undocumented and somewhat dangerous... */

	if (! strcmp(argv[i], "-t"))
	  { /**** timer interval	****/
	    if(argc > ++i) TIMER_INTERVAL = atoi(argv[i]);
	    else
	      {
		fprintf(stderr, "Missing argument after -t\n");
		exit(1);
	      }
	  }

	else if ((! strcmp(argv[i], "-m")) || (!strcmp(argv[i], "-M")))
	  { /**** sysout size	****/
	    if(argc > ++i) sysout_size = atoi(argv[i]);
	    else
	      {
		fprintf(stderr, "Missing argument after -m\n");
		exit(1);
	      }
	  }

	else if ((! strcmp(argv[i], "-nf")) || (!strcmp(argv[i], "-NF")))
	  { /****  Don't fork (for dbxing)	****/
	    please_fork = 0;
	  }

	else if ( ! strcmp(argv[i], "-INIT"))
	  { /*** init sysout, no packaged */
	    for_makeinit = 1;
	  }
	else if (! strcmp(argv[i], "-k"))
	  { /**** security key	****/
	    if(argc > ++i) (void)strcpy(keystring, argv[i]);
	    else
	      {
		fprintf(stderr, "Missing argument after -k\n");
		exit(1);
	      }
	  }
#ifdef DOS
	else if ((strcmp(argv[i], "-vga") == 0) ||
		 (strcmp(argv[i], "-VGA") == 0))
	  {
	    dosdisplaymode = 1;
	  }
	else if ((strcmp(argv[i], "-vesa102") == 0) ||
		 (strcmp(argv[i], "-VESA102") == 0))
	  {
	    dosdisplaymode = 0x102;
	  }
	else if ((strcmp(argv[i], "-vesa104") == 0) ||
		 (strcmp(argv[i], "-VESA104") == 0))
	  {
	    dosdisplaymode = 0x104;
	  }
#endif /* DOS */

	/* Can only do this under SUNOs, for now */
	else if (! strcmp(argv[i], "-E"))
	  { /**** ethernet info	****/
#ifdef NOETHER
#else
	    int b0, b1, b2, b3, b4, b5;
	    if(argc > ++i && sscanf(argv[i],"%d:%x:%x:%x:%x:%x:%x:%s",&ether_fd,
			&b0, &b1, &b2, &b3, &b4, &b5, snit.snit_ifname) == 8)
	      {
		ether_host[0] = b0; ether_host[1] = b1;
		ether_host[2] = b2; ether_host[3] = b3;
		ether_host[4] = b4; ether_host[5] = b5;
	      }
	    else
	      {
		fprintf(stderr, "Missing or bogus -E argument\n");
		ether_fd = -1;
		exit(1);
	      }
#endif /* NOETHER */

	  }
      }

    if ( keystring )		/* key given, so safe to copy */
      strcpy ( keytyped, keystring );
    if ( keytester(keystring) ) { /* keytester destroys keystring */
	    fprintf (stderr, "Sorry, invalid or expired access key.\n" );
	    fprintf (stderr, "Key tried: %s\n", keytyped );
	    exit();
	    }

#ifndef NOETHER
    init_ether();			/* modified by kiuchi Nov. 4 */
#endif /* NOETHER */


#ifdef FORKCOMM
    /* Fork Unix was called in kickstarter; if we forked, look up the */
    /* pipe handles to the subprocess and set them up.		      */

    if(FindUnixPipes()) /* must call the routine to allocate storage, */
      {			/* in case we're re-starting a savevm w/open ptys */
	if(please_fork)
	  fprintf(stderr,
		  "Failed to find UNIXCOMM file handles; no processes\n");
      }
#else
    /* Fork Unix Interface subprocess before we create anything big;   */
    /* interrupts need to be blocked here so subprocess won't see them */
    /* This should actually live in the kickstarter... */

#ifndef DOS
    if(please_fork){int_block(); fork_Unix();}    
#endif /* DOS */
#endif /* FORKCOMM */
#if defined(DOS) || defined (XWINDOW)
    make_dsp_instance(currentdsp, 0, 0, 0, 1 );	/* All defaults the first time */
#endif /* DOS || XWINDOW */

    /* Load sysout to VM space and returns real sysout_size(not 0) */
    sysout_size= sysout_loader(sysout_name, sysout_size); 
printf("sysout size = 0x%x   ",sysout_size);
    build_lisp_map();		/* built up map */

    init_ifpage(sysout_size);	/* init interface page */
    init_iopage();
    init_miscstats();
#ifdef NEW_STORAGE
    init_storage();
#endif /* NEW_STORAGE */

    set_cursor();

        /* file system directory enumeration stuff */
    if (!init_finfo())
      {
	fprintf(stderr, "Cannot allocate internal data.\n");
	exit(1);
      }

    /* Get OS message to ~/lisp.log and print the message to prompt window */
    if(!for_makeinit)
      {
#ifdef SUNDISPLAY
	mess_init();
#endif /* SUNDISPLAY */

	init_keyboard(0); /* can't turn on the keyboard yet or you will die
			     in makeinit.  Pilotbitblt will turn it on if
			     you used the proper switches when building LDE.
				JDS -- 1/18/90 also BITBLTSUB does it now. */
      }

#ifdef I386
/* 80387 initialization */
    asm(".data");
    asm(".align 4");
    asm("__FP_trunc:");
    asm("	.globl __FP_trunc");
    asm(".byte 0x3f");
    asm(".byte 0x0c");
    asm("__FP_round:");
    asm("	.globl	__FP_round");
    asm(".byte 0x3f");
    asm(".byte 0x00");
    asm(".text");
    asm("fldcw __FP_round");	
#endif /* I386 */




#ifdef RS6000
    /****************************************************/
    /*  For the IBM RS/6000, we have to initialize its	*/
    /*  notion of the time zone, so gettimeofday works	*/
    /****************************************************/

    tzset();
#endif /* RS6000 */
#ifdef DOS
    tzset();
#endif /* DOS */


    /* now start up lisp */
    start_lisp();

#ifdef DOS
   printf("returned from start_lisp.\n"); getch();
#endif

  }




/************************************************************************/
/*									*/
/*		  	   s t a r t _ l i s p				*/
/*									*/
/*	This is the function that actually starts up the lisp emulator.	*/
/*									*/
/*									*/
/************************************************************************/

start_lisp()
  {
    DLword *freeptr,*next68k;

    TopOfStack=0;
    Error_Exit = 0;

    TPRINT(("Enter start_lisp\n"));
    MState->pvar = (DLword *) Addr68k_from_LADDR( STK_OFFSET
						 | InterfacePage->currentfxp)
		              + FRAMESIZE;

    freeptr = next68k = Addr68k_from_LADDR(STK_OFFSET | CURRENTFX->nextblock);
  
    if (GETWORD(next68k) != STK_FSB_WORD)
      error("Starting Lisp: Next stack block isn't free!");

    while(GETWORD(freeptr)==STK_FSB_WORD)
      EndSTKP = freeptr = freeptr + GETWORD(freeptr+1);

    CurrentStackPTR =next68k -2;

    FastRetCALL;

    /* JRB - The interrupt intitialization must be done right before */
    /*       entering the bytecode dispatch loop; interrupts get     */
    /*       unblocked here 					     */
    int_init();
    dispatch();
  }




/************************************************************************/
/*									*/
/*			m a k e p a t h n a m e				*/
/*									*/
/*									*/
/*									*/
/************************************************************************/

makepathname(src, dst)
  register char	*src;
  register char	*dst;				
  {
    register int len;
    register char	*base, *cp;
    register struct passwd *pwd;
    char	name[MAXPATHLEN];

    base = src;
    switch ( *base )
      {
	case '.':
#ifdef DOS
	  if(getcwd(dst, MAXPATHLEN) == 0)
#else
	  if(getwd(dst) == 0)
#endif /* DOS */
	    { /* set working directory */
#ifdef FSERROR
	      *Lisp_errno = errno;
#endif
	      return(0);
	    }
	  switch ( *(base+1) )
	    {					
	      case '.':
		if(*(base+2) == '/')
		  { /* Now, base == "../xxxx" */
		    cp = rindex(dst,(int)'/');
		    if(cp == 0) return(0);
		    *cp = '\0';
		    strcat(dst, base+2);
		    return(1);					
		  }
		else return(0);			
	      case '/':
		/* Now, base == "./xxx" */
		strcat(dst, base+1);			
		return(1);						
	      default:							
		return(0);				
	    }
	case '~':
#ifdef FSERROR
	  ERRSETJMP(0);
#else
	  SETJMP(0);
#endif
	  if( *(base+1) == '/')
	    {
	      /* path is "~/foo" */
#ifdef DOS
	pwd = 0;
#else
	      TIMEOUT( pwd = getpwuid( getuid()) );
#endif /* DOS */
	      if(pwd == NULL)
		{				
#ifdef FSERROR
		  *Lisp_errno = errno;
#endif
		  return(0);
		}
#ifndef DOS
	      sprintf(dst, "%s%s", pwd->pw_dir, base+1);
#endif
	      return(1);
	    }
	  else
	    {
	      /* path is "~foo/" */
	      if((cp=index(base+1, '/')) == 0) return(0);
	      else
		{
		  len = (int)cp - (int)base -1;
		  strncpy(name, base+1, len);
		  name[len] = '\0';
#ifndef DOS
		  TIMEOUT( pwd = getpwnam(name) );
#endif /* DOS */
		  if(pwd == NULL)
		    {				
#ifdef FSERROR
		      *Lisp_errno = errno;
#endif
		      return(0);
		    }
#ifndef DOS
		  sprintf(dst, "%s%s", pwd->pw_dir, cp);
#endif /* DOS */
		  return(1);	
		}
	    }
	default:
	  strcpy(dst, src);					
	  return(1);							
      }
  }
