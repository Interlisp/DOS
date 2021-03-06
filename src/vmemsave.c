/* @(#) vmemsave.c Version 2.38 (4/21/92). copyright venue & Fuji Xerox  */
static char *id = "@(#) vmemsave.c	2.38 4/21/92         (venue & Fuji Xerox)";



/************************************************************************/
/*									*/
/*	(C) Copyright 1989, 1990, 1991 Venue. All Rights Reserved.	*/
/*	Manufactured in the United States of America.			*/
/*									*/
/*	The contents of this file are proprietary information 		*/
/*	belonging to Venue, and are provided to you under license.	*/
/*	They may not be further distributed or disclosed to third	*/
/*	parties without the specific permission of Venue.		*/
/*									*/
/************************************************************************/



/*
*	vmemsave.c
*
*
*/

#include <stdio.h>
#include <sys/types.h>
#ifndef DOS
#include <sys/file.h>
#endif /* DOS */
#include <sys/stat.h>
#ifndef DOS
#include <sys/param.h>
#include <sys/dir.h>
#ifndef AIX
#include <sys/vfs.h>
#endif /* AIX */

#include <pwd.h>
#endif /* DOS */
#include <errno.h>
#include <signal.h>
#include <setjmp.h>
#ifdef RISCOS
#include <dirent.h>
#endif /* RISCOS */

#ifdef ISC
#include <dirent.h>
#include <unistd.h>
#include <sys/fcntl.h>
#endif /* ISC */

#ifdef DOS
#include <direct.h>
#include <fcntl.h>
#include <stdlib.h>
#define MAXPATHLEN _MAX_PATH
#define MAXNAMLEN _MAX_PATH
#define alarm(x) 1
#endif /* DOS */


#ifndef NOPIXRECT
#include <sunwindow/win_cursor.h>
#endif /* NOPIXRECT */


#include "hdw_conf.h"
#include "lispemul.h"
#include "lispmap.h"
#include "lspglob.h"
#include "ifpage.h"
#include "vmemsave.h"
#include "timeout.h"
#include "adr68k.h"
#include "lsptypes.h"
#include "locfile.h"
#include "dbprint.h"

#define	COMPLETESYSOUT	NIL
#define	BADFILENAME	S_POSITIVE | 1	
#define	NOFILESPACE	S_POSITIVE | 2	
#define FILECANNOTOPEN	S_POSITIVE | 3
#define FILECANNOTSEEK	S_POSITIVE | 4
#define FILECANNOTWRITE	S_POSITIVE | 5
#define FILETIMEOUT	S_POSITIVE | 6

struct stat     DEBUG_stat;

extern int LispWindowFd;
extern struct pixrect *CursorBitMap, *InvisibleCursorBitMap;
extern struct cursor CurrentCursor, InvisibleCursor;
extern IFPAGE *InterfacePage;

char	*rindex();

#undef roundup
#define roundup( a, b) ((((unsigned)(a)+(b)-1)/(b))*(b))

extern int	*Lisp_errno;
extern int	Dummy_errno;	/* Used if errno cell isn't provided by Lisp.*/
extern int      please_fork;


/************************************************************************/
/*									*/
/*			l i s p s t r i n g p				*/
/*									*/
/*	Predicate:  Is the argument (which must be a Lisp 1-d array)	*/
/*		    a lisp string?  i.e., are its elements char's?	*/
/*									*/
/************************************************************************/

lispstringP(Lisp)
  LispPTR	Lisp;
  {
    switch( ((OneDArray *)(Addr68k_from_LADDR(Lisp)))->typenumber )
      {
	case THIN_CHAR_TYPENUMBER:
	case FAT_CHAR_TYPENUMBER:   return(1);

	default: return(0);
      }
  }



/************************************************************************/
/*									*/
/*			v m e m _ s a v e 0				*/
/*									*/
/*	Implements the VMEMSAVE subr.  Write out the current lisp	*/
/*	lisp image to the specified file.  If the sysout file-name	*/
/*	is explicitly specified, the directory on which the file	*/
/*	resides is exactly (?) an existing directory.  This is		*/
/*	guaranteed by the Lisp code, \MAIKO.CHECKFREEPAGE in LLFAULT.	*/
/*									*/
/*	If the file argument is nill, the default sysout-file name,	*/
/*	"~/lisp.virtualmem", is used, subject to override by the	*/
/*	LDEDESTSYSOUT UNIX-environment variable.			*/
/*									*/
/*									*/
/*									*/
/*									*/
/* Argument:	LispPTR	*args	args[0]					*/
/*			 The file name in Lisp format specifying a	*/
/*			 file to which the current Lisp image should	*/
/*			 be flushed or Lisp NIL.			*/
/*									*/
/* Value:	If succeed, returns NIL, otherwise one of the		*/
/*		following Lisp integers, indicating the reason.		*/
/*									*/
/*			1	BADFILENAME				*/
/*			2	NOFILESPACE				*/
/*			3	FILECANNOTOPEN				*/
/*			4	FILECANNOTSEEK				*/
/*			5	FILECANNOTWRITE				*/
/*			6	FILETIMEOUT				*/
/*									*/
/* Side Effect:	None.							*/
/*									*/
/************************************************************************/

vmem_save0(args)
  register LispPTR	*args;
  {
    register char	*def;
    char	pathname[MAXPATHLEN], sysout[MAXPATHLEN], host[MAXNAMLEN];
#ifdef DOS
    char *pwd;
#else
    struct passwd	*pwd;
#endif /* DOS */
    char		*getenv();
    char		*index();
    
    Lisp_errno = &Dummy_errno;
	
    if((args[0] != NIL) && lispstringP(args[0]))
      {
	/* Check of lispstringP is safer for LispStringToCString */
	LispStringToCString(args[0], pathname, MAXPATHLEN);
	separate_host(pathname, host);
	if (!unixpathname(pathname, sysout, 0, 0)) return(BADFILENAME);
	return(vmem_save(sysout));
      }
    else
      {
	if((def = getenv("LDEDESTSYSOUT")) == 0)
	  {
#ifdef DOS
	pwd = getcwd(pwd,MAXNAMLEN);
	strcpy(sysout,pwd);
	    strcat(sysout, "/lisp.vm");
#else
	    pwd = getpwuid(getuid()); /* NEED TIMEOUT */
	    if (pwd == (struct passwd *)NULL) return(FILETIMEOUT);
	    strcpy(sysout, pwd->pw_dir);
	    strcat(sysout, "/lisp.virtualmem");
#endif /* DOS */
	  }
	 else
	  {
	    if (*def == '~' &&
	        (*(def + 1) == '/' || *(def + 1) == '\0'))
	      {
#ifdef DOS
		pwd = getcwd(pwd,MAXNAMLEN);
		strcpy(sysout,pwd);
#else
		pwd = getpwuid(getuid()); /* NEED TIMEOUT */
		if (pwd == (struct passwd *)NULL) return(FILETIMEOUT);
		strcpy(sysout, pwd->pw_dir);
#endif /* DOS */
		strcat(sysout, def + 1);
	      }
	    else
	      {
		strcpy(sysout, def);
	      }
	  }
	return(vmem_save(sysout));
      }
  }



/************************************************************************/
/*									*/
/*				v m e m _ s a v e			*/
/*									*/
/*									*/
/*									*/
/************************************************************************/

/*		
 * Argument:	char	*sysout_file_name
 *			 The file name in UNIX format specifying a file to which
 *			 the current Lisp image should be flushed.
 *
 * Value:	If succeed, returns Lisp NIL, otherwise one of the following Lisp integer
 *		indicating the reason of failure.
 *
 *			1	BADFILENAME
 *			2	NOFILESPACE
 *			3	FILECANNOTOPEN
 *			4	FILECANNOTSEEK
 *			5	FILECANNOTWRITE
 *			6	FILETIMEOUT
 *
 * Side Effect:	None.
 *
 * Description:
 *
 * Flush out the current Lisp image to the specified file.
 */

vmem_save(sysout_file_name)
  register char	*sysout_file_name;
  {
    int		sysout;	/* SysoutFile descriptor */
    register DLword	*fptovp;	/* FPTOVP */
    int	vmemsize;	/* VMEMSIZE */
    register int	i;
    char	tempname[MAXPATHLEN];
 /* * *   struct statfs	fsbuf; * * */
    char	*cp;
    register int	rval;
    DLword *bmptr;
    extern int ScreenLocked;
    extern DLword *EmCursorX68K;
    extern DLword *EmCursorY68K;
    extern DLword NullCursor[];	
    extern DLword *EmCursorBitMap68K;

    InterfacePage->minbversion = 15000; /* Mark this sysout for new emulator */
			
    /* remove cursor image from screen */

#ifdef SUNDISPLAY
#ifdef OLD_CURSOR
    win_setcursor( LispWindowFd, &InvisibleCursor);
#else
    ScreenLocked=T;
    taking_mouse_down();
/*	bmptr = EmCursorBitMap68K;
    EmCursorBitMap68K= NullCursor;*/
#endif /* OLD_CURSOR */

#elif DOS
    /*  For DOS, must also take the mouse cursor away (it's  */
    /*  written into the display-region bitmap).	     */
    ScreenLocked=T;
    taking_mouse_down();
/*	bmptr = EmCursorBitMap68K;
    EmCursorBitMap68K= NullCursor;*/
#endif /* SUNDISPLAY */


    /* set FPTOVP */
    fptovp = FPtoVP + 1;

    /* set VMEMSIZE */
    vmemsize = InterfacePage->nactivepages;

/*	[HH:6-Jan-89]
	Sequnce of save image
	(1) Sysout image is saved to a temporary file, tempname.
	(2) if a specified file, sysout_file_name, is exist, the file is removed.
	(3) the temporary file is renamed to the specified file.
*/

    SETJMP(FILETIMEOUT);
    sprintf(tempname, "%s-temp", sysout_file_name);

	/* Confirm protection of specified file by open/close */

    TIMEOUT(sysout=open(sysout_file_name, O_WRONLY, 0666));
    if (sysout == -1)
      {
	/* No file error skip return. */
	if(errno != 2) return(FILECANNOTOPEN); /* No such file error.*/
      }
    else TIMEOUT( rval = close(sysout) );

    /* open temp file */
    TIMEOUT(sysout=open(tempname, O_WRONLY | O_CREAT | O_TRUNC, 0666));
    if (sysout == -1)
      {
	err_mess("open", errno);
	return(FILECANNOTOPEN);
      }


    InterfacePage->machinetype = KATANA;
/** CHANGES FOR NEW STORAGE **/
#ifdef NEW_STORAGE
    /* Restore storagefull state */
    if(((*STORAGEFULLSTATE_word) & 0xffff)== SFS_NOTSWITCHABLE){
	/* This sysout uses only 8 Mbyte lisp space.
	   It may be able to use this SYSOUT which has more than
	   8 Mbyte lisp space.
	   To enable to expand lisp space, \\STORAGEFULLSTATE
	   should be NIL.
	*/
	*STORAGEFULLSTATE_word = NIL;
	InterfacePage->storagefullstate=NIL;
    }
    else {
	/*  Otherwise, just restore storagefullstate in IFPAGE */
    	InterfacePage->storagefullstate= 
		(*STORAGEFULLSTATE_word) & 0xffff ;
    }
/** CHANGES FOR NEW STORAGE END**/
#endif NEW_STORAGE

    /*printf("start vmemsave\n"); */

    /* First, sort fptovp table, trying to get pages contiguous */
    /* (doesn't work with byte swapping in effect. ) */
#ifndef BYTESWAP
    sort_fptovp(fptovp, vmemsize);
#endif /* BYTESWAP */

    /* store vmem to sysoutfile */

    for (i = 0; i < vmemsize; i++)
      {
	if (GETBASEWORD(fptovp,i) != 0177777)
	  {
	    int oldfptovp = GETBASEWORD(fptovp, i);
	    int contig_pages = 0;
#ifdef BYTESWAP
	    int pc;
#endif /* BYTESWAP */

	    register char *base_addr;

	    TIMEOUT( rval=lseek(sysout, i * BYTESPER_PAGE, 0) );
	    if (rval == -1) 
	      {
		err_mess("lseek", errno);
		return(FILECANNOTSEEK);
	      }
	    base_addr = (char *)(Lisp_world + (GETBASEWORD(fptovp,i) * DLWORDSPER_PAGE));

	    /* Now, let's see how many pages we can dump */
	    while(GETBASEWORD(fptovp,i) == oldfptovp
		  && i < vmemsize) {
	      contig_pages++, oldfptovp++, i++;
	    }
	    i--;	/* Previous loop always overbumps i */
	    DBPRINT(("%4d: writing %d pages from %x\n",
		    i, contig_pages, base_addr - (char*)Lisp_world));

#ifdef BYTESWAP
	    word_swap_page(base_addr,
			    contig_pages*BYTESPER_PAGE/4);
#endif /* BYTESWAP */

	    { int oldTT = TIMEOUT_TIME;
	      /* As we can spend longer than TIMEOUT_TIME doing a big
		 write, we adjust the timeout temporarily here */
	      TIMEOUT_TIME += contig_pages>>5;
	      TIMEOUT( rval=write(sysout,
				  base_addr,
				  contig_pages * BYTESPER_PAGE) );
	      TIMEOUT_TIME = oldTT;
	    }
#ifdef BYTESWAP
	    word_swap_page(base_addr,
			    contig_pages*BYTESPER_PAGE/4);
#endif /* BYTESWAP */

	    if (rval == -1)
	      {
		err_mess("write", errno);
		return(FILECANNOTWRITE);
	      }
	  }
      }



    /* seek to IFPAGE */
    TIMEOUT(rval=lseek(sysout, (long)FP_IFPAGE, 0));
    if (rval == -1)
      {
	err_mess("lseek", errno);
	return(FILECANNOTSEEK);
      }

#ifdef BYTESWAP
	    word_swap_page((char *) InterfacePage,
			    BYTESPER_PAGE/4);
#endif /* BYTESWAP */

    TIMEOUT(rval=write(sysout, (char *) InterfacePage, BYTESPER_PAGE) );

#ifdef BYTESWAP
	    word_swap_page((char *) InterfacePage,
			    BYTESPER_PAGE/4);
#endif /* BYTESWAP */

    if (rval == -1)
      {
		err_mess("write", errno);
		return(FILECANNOTWRITE);
	}
    
    TIMEOUT( rval = close(sysout) );
    if(rval == -1)
      {
	return(FILECANNOTWRITE);
      }

    TIMEOUT(rval = unlink(sysout_file_name));
    if (rval == -1) 
      {
	/* No file error skip return. */
	if(errno != 2)	/* No such file error.*/ return(FILECANNOTOPEN);
      }

    TIMEOUT( rval=rename(tempname, sysout_file_name) );
    if( rval== -1)
      {
	fprintf(stderr,"sysout is saved to temp file, %s.", tempname);
	return(FILECANNOTWRITE);
      }

    /* restore cursor image to screen */
#ifdef SUNDISPLAY
#ifdef OLD_CURSOR
    win_setcursor( LispWindowFd, &CurrentCursor);
#else
    ScreenLocked=T;
    /*EmCursorBitMap68K = bmptr ;*/
    taking_mouse_up(*EmCursorX68K,*EmCursorY68K);
    ScreenLocked=NIL;
#endif /* OLD_CURSOR */

#elif DOS
    /* Must also put the mouse back. */
    ScreenLocked=T;
    /*EmCursorBitMap68K = bmptr ;*/
    taking_mouse_up(*EmCursorX68K,*EmCursorY68K);
    ScreenLocked=NIL;

#endif /* SUNDISPLAY */


    /*printf("vmem is saved completely.\n");*/
    return(COMPLETESYSOUT);
  }




/************************************************************************/
/*									*/
/*			l i s p _ f i n i s h				*/
/*									*/
/*	Kill all forked sub-processes before exiting.			*/
/*									*/
/************************************************************************/

extern int UnixPID;

/* Make sure that we kill off any Unix subprocesses before we go away */

lisp_finish()
  {
    char d[4];

    DBPRINT(("finish lisp_finish\n"));

    if( please_fork )
      {		/* if lde runs with -NF(No fork), */
		/* following 5 lines don't work well. */
	d[0] = 'E';
	d[3] = 1;
	/* These only happen if the fork really succeeded:
	/* if (UnixPipeOut >= 0) write(UnixPipeOut, d, 4); */
	/* if (UnixPipeIn >= 0 read(UnixPipeIn, d, 4);*/	/* Make sure it's finished */
	/* if (UnixPID >= 0) kill(UnixPID, SIGKILL);*/	/* Then kill fork_Unix itself */
      }
    device_before_exit();
    exit(1);
	
  }




/************************************************************************/
/*									*/
/*			s o r t _ f p t o v p				*/
/*									*/
/*	Sort the entries in the file-page-to-virtual-page table,	*/
/*	to try to make a sysout file that has contiguous runs of	*/
/*	virtual pages in it, for speed.					*/
/*									*/
/************************************************************************/
#ifndef BYTESWAP
int twowords(i,j)	/* the difference between two  DLwords. */
  DLword *i, *j;
  {
    return(*i - *j);
  }


#define FPTOVP_ENTRY (FPTOVP_OFFSET >> 8)

sort_fptovp(fptovp, size)
  DLword *fptovp;
  int size;
  {
    int oldloc, newloc, oldsize, i;
    DLword *fptr;

    for(fptr = fptovp, i=0;
        *fptr != FPTOVP_ENTRY && i < size;
        fptr++, i++);

    if(*fptr != FPTOVP_ENTRY)
      {
	DBPRINT((stderr, "Couldn't find FPTOVP_ENTRY; not munging\n"));
	return;
      }
    oldloc = fptr - fptovp;

    /* Found old fptovp table location, now sort the table */
    qsort(fptovp, size, sizeof(DLword), twowords);

ONE_MORE_TIME: /* Tacky, but why repeat code? */

    /* Look up FPTOVP_ENTRY again; if it's moved, need to shuffle stuff */
    for(fptr = fptovp, i=0;
        *fptr != FPTOVP_ENTRY && i < size;
        fptr++, i++);

    if(*fptr != FPTOVP_ENTRY) error("Couldn't find FPTOVP_ENTRY second time!\n");
    newloc = fptr - fptovp;

    /* Supposedly all we have to do is adjust the fptovpstart and nactivepages
       the ifpage */
    InterfacePage->fptovpstart += (newloc - oldloc);
    oldsize = size;
    for(fptr = fptovp + (size-1); *fptr == 0xffff;
        fptr--, InterfacePage->nactivepages--, size--);

    if(size != oldsize)
      DBPRINT(("Found %d holes in fptovp table\n", oldsize - size));

      /* Sanity check; it's just possible there are duplicate entries... */
      {
	int dupcount = 0;
	for(fptr = fptovp, i=1;
	    i<size;
	    i++, fptr++)
	if(*fptr == *(fptr+1))
	  {
	    dupcount++;
	    *fptr = 0xffff;
	  }

	/* if duplicates were found, resort to squeeze them out, then mung the
           size and fptovpstart again (spaghetti-code, HO!) */
	if(dupcount)
	  {
	    qsort(fptovp, size, sizeof(DLword), twowords);
	    oldloc = newloc;
	    DBPRINT((stderr, "%d duplicates found\n", dupcount));
	    goto ONE_MORE_TIME;
	  }
      }
  }
#endif /* BYTESWAP */
