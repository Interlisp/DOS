/* @(#) ldsout.c Version 1.2 (4/22/92). copyright venue & Fuji Xerox  */
static char *id = "@(#) ldsout.c	1.2 4/22/92	(venue & Fuji Xerox)";



/************************************************************************/
/*									*/
/*	(C) Copyright 1989, 1990, 1991, 1992 Venue. All Rights Reserved.	*/
/*	Manufactured in the United States of America.			*/
/*									*/
/*	The contents of this file are proprietary information 		*/
/*	belonging to Venue, and are provided to you under license.	*/
/*	They may not be further distributed or disclosed to third	*/
/*	parties without the specific permission of Venue.		*/
/*									*/
/************************************************************************/


#include <stdio.h>
#include <sys/types.h>
#ifndef DOS
#include <sys/file.h>
#endif /* DOS */
#include <sys/stat.h>
#include <errno.h>

#ifdef ISC
#include <sys/fcntl.h>
#endif /* ISC */

#ifdef DOS
#include <fcntl.h>
#endif /* DOS */


#include "adr68k.h"
#include "lispemul.h"
#include "lsptypes.h"
#include "lispmap.h"
#include "lspglob.h"
#include "ifpage.h"
#include "dbprint.h"

#ifdef AIXPS2
#include "inlnPS2.h"
#endif /* AIXPS2 */

#ifdef ISC
#include "inlnPS2.h"
#endif /* ISC */


#define	IFPAGE_ADDRESS	512
#define DEFAULT_MAX_SYSOUTSIZE		32  /* in Mbyte */
#define DEFAULT_PRIME_SYSOUTSIZE	8
#define MBYTE	0x100000 /* 1 Mbyte */
extern int	errno;

/* Flag for indication whether process space 
  is going to expand or not */
int Storage_expanded;  /*  T or NIL */


  /* RISCOS doesn't have valloc, and malloc works OK there. */
#ifdef SYSVONLY
#define valloc malloc
#endif /* SYSVONLY */


char *valloc();


/************************************************************************/
/*									*/
/*			s y s o u t _ l o a d e r			*/
/*									*/
/*	Load the sysout file into memory.				*/
/*									*/
/************************************************************************/

int sysout_loader(sysout_file_name, sys_size)
  char	*sysout_file_name;
  int	sys_size;   /* sysout size in megabytes */
  {
    int             sysout;	/* SysoutFile descriptor */

    IFPAGE         ifpage;	/* IFPAGE */

    char           *fptovp_scratch;	/* scratch area for FPTOVP */
    DLword         fptovp[0x10000];	/* FPTOVP */
    long           fptovp_offset;	/* FPTOVP start offset */

    char           *lispworld_scratch;	/* scratch area for lispworld */
    long            lispworld_offset;	/* lispworld offset */

    unsigned        sysout_size;/* sysout size in page */
    struct stat     stat_buf;	/* file stat buf */
    int             i, vp;

    int             machinetype;
    char            errmsg [ 255 ];

    machinetype = 0;

    /* Checks for specifying the process size (phase I) */
    /* If sys_size == 0 figure out the proper size later */
    if((sys_size != 0) && (sys_size < DEFAULT_PRIME_SYSOUTSIZE))
      {
	perror("You have to specify more than 8MB for process size");
	exit(-1);
      }
    else if ( sys_size > DEFAULT_MAX_SYSOUTSIZE)
      {
	perror("You have to specify less than 32MB for process size");
	exit(-1);
      }

    /*
     * first read the IFPAGE(InterfacePage)
     */

    /* open SysoutFile */
    sysout = open(sysout_file_name, O_RDONLY, NULL);
    if (sysout == -1)
      {
	sprintf( errmsg,
                 "sysout_loader: can't open sysout file: %s",
		 sysout_file_name);
	perror( errmsg );
	exit(-1);
      }

    /* seek to IFPAGE */
    if (lseek(sysout, IFPAGE_ADDRESS, 0) == -1)
      {
	perror("sysout_loader: can't seek to IFPAGE");
	exit(-1);
      }

    /* reads IFPAGE into scratch_page */
    if (read(sysout, &ifpage, sizeof(IFPAGE)) == -1)
      {
	perror("sysout_loader: can't read IFPAGE");
	exit(-1);
      }

#ifdef BYTESWAP
    word_swap_page(&ifpage, (3+sizeof(IFPAGE))/4);
#endif

#ifdef NEW_STORAGE
    if(sys_size == 0)  /* use default or the previous one */
      {
	if(ifpage.process_size == 0) /* Pure LISP.SYSOUT */
	  sys_size = DEFAULT_MAX_SYSOUTSIZE;  /* default for pure SYSOUT */
	else
	  sys_size = ifpage.process_size; /* use previous one */
      }
    /* Checks for specifying the process size (phase II) */
    if((ifpage.storagefullstate == SFS_ARRAYSWITCHED) ||
       (ifpage.storagefullstate == SFS_FULLYSWITCHED)) 
     {
	/* Storage may be allocated from Secondary space */
	/* Therefore you can not change \\DefaultSecondMDSPage */
	if(ifpage.process_size != sys_size)
	{
	  char tmp[200];
	  sprintf(tmp, "\nsysout loader: Error, secondary space in use. You can't specify size.\nProcess size = %d\nSys size = %d\n", ifpage.process_size, sys_size);
#ifdef DOS
	  /* Note that we have an initialized display by now. */
	  /* Hence we have to observe the display protocol. */
	  VESA_errorexit( tmp );
#else
	fprintf(stderr,"sysout_loader: You can't specify the process size.\n");
	fprintf(stderr,"Because, secondary space is already used.\n");
	exit(-1);
#endif /* DOS */
	}
	/*Can use this sys_size as the process size */
	/* The sys_size should be same as the previous one */
	Storage_expanded=NIL;
     }
    else
     {  /* assumes that we can expand the process space */
	Storage_expanded=T;
	/* You can use secondary space , though it was STORAGEFULL
	   So, STORAGEFULL may be set to NIL later  */
     }
#else
	if(sys_size == 0) sys_size = DEFAULT_MAX_SYSOUTSIZE;
#endif /* NEW_STORAGE */


    /* allocate Virtual Memory Space */

    lispworld_scratch = valloc(sys_size * MBYTE);

    if (lispworld_scratch == NULL)
      {
	fprintf(stderr,"sysout_loader: can't allocate Lisp %dMBytes VM \n",
								   sys_size);
	exit(-1);
     }

    /* now you can access lispworld */
    Lisp_world = (DLword *) lispworld_scratch;

    DBPRINT(("VM allocated = 0x%x at 0x%x\n", sys_size * MBYTE, Lisp_world));
    DBPRINT(("Native Load Address = 0x%x\n", native_load_address));


    /*
     * get FPTOVP location and SysoutFile size
     */

    /* get FPTOVP location from IFPAGE */
    fptovp_offset = ifpage.fptovpstart;

    DBPRINT(("FPTOVP Location %d \n", fptovp_offset));

    /* get sysout file size in halfpage(256) */
    if (fstat(sysout, &stat_buf) == -1)
      {
	perror("sysout_loader: can't get sysout file size");
	exit(-1);
      }

    sysout_size = stat_buf.st_size / BYTESPER_PAGE * 2;

    DBPRINT(("sysout size / 2 = 0x%x\n", sysout_size / 2));
    DBPRINT(("vmem size = 0x%x\n", ifpage.nactivepages));

    /* do some simple checks to see if this is really a sysout */
    if(ifpage.key != IFPAGE_KEYVAL)
      {
	printf("sysout_loader: %s isn't a sysout:\nkey is %x, should be %x\n",
			sysout_file_name, ifpage.key, IFPAGE_KEYVAL);
	exit(1);
      }

    machinetype = ifpage.machinetype;

    if(( stat_buf.st_size & 0x1ff) != 0)
	printf("CAUTION::sysout & 0x1ff = 0x%x\n",
		stat_buf.st_size & BYTESPER_PAGE);

    if(ifpage.nactivepages != ( sysout_size /2) )
      {
	printf("sysout_loader:IFPAGE says sysout size is %d pages,\n",
               ifpage.nactivepages);
	printf("but sysout file size is %d pages.\n",sysout_size /2);
	exit(-1);
      }

    /*
     * Now get FPTOVP
     */

    /* seek to FPTOVP */
    fptovp_offset = (fptovp_offset - 1) * BYTESPER_PAGE + 2;
    if (lseek(sysout, fptovp_offset, 0) == -1)
      {
	perror("sysout_loader: can't seek to FPTOVP");
	exit(-1);
      }
    /* read FPTOVP */
    if (read(sysout, fptovp, sysout_size) == -1)
      {
	perror("sysout_loader: can't read FPTOVP");
	exit(-1);
      }
#ifdef BYTESWAP
    word_swap_page(fptovp, (sysout_size/4)+1);
#endif
    /*
     * Initialize the display (note now passing 68k address!!!)
     */
    init_display2(Addr68k_from_LADDR(DISPLAY_OFFSET), 65536 * 16 * 2);

    /* read sysout file to lispworld */

    for (i = 0; i < (sysout_size / 2); i++)
      {
	if (GETBASEWORD(fptovp,i) != 0177777)
	  {
	    if (lseek(sysout, i * BYTESPER_PAGE, 0) == -1)
	      {
		perror("sysout_loader: can't seek sysout file");
		exit(-1);
	      };
	    lispworld_offset = GETBASEWORD(fptovp,i) * BYTESPER_PAGE;
	    if (read(sysout, lispworld_scratch +
			     lispworld_offset , BYTESPER_PAGE) == -1)
	      {
		printf("sysout_loader: can't read sysout file at %d\n", i);
		printf("               offset was 0x%x (0x%x pages).\n",
		       lispworld_offset, GETBASEWORD(fptovp,i));
		perror("read() error was");
		exit(-1);
	      };
#ifdef BYTESWAP
            word_swap_page(lispworld_scratch+lispworld_offset, 128);
#endif

	  };
      }

    DBPRINT(("sysout file is read completely.\n"));
#if (defined(DISPLAYBUFFER) || defined(XWINDOW) || defined(DOS))
    TPRINT(("Flushing display buffer...\n"))
	printf("before flush, sys_size = %d   ", sys_size); getchar();
    flush_display_buffer();
	printf("after flush, sys_size = %d  ", sys_size); getchar();
    TPRINT(("After Flushing display buffer\n"))
#endif /* DISPLAYBUFFER || XWINDOW || DOS */


    close(sysout);
    return(sys_size);
  }
