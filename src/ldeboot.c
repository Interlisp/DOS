/* @(#) ldeboot.c Version 1.11 (4/21/92). copyright Venue & Fuji Xerox  */
static char *id = "@(#) ldeboot.c	1.11 4/21/92		(Venue & Fuji Xerox)";	



/************************************************************************/
/*									*/
/*	Copyright 1989, 1990 Venue, Fuji Xerox Co., Ltd, Xerox Corp.	*/
/*									*/
/*	This file is work-product resulting from the Xerox/Venue	*/
/*	Agreement dated 18-August-1989 for support of Medley.		*/
/*									*/
/************************************************************************/


#include <stdio.h>
#include <ctype.h>
#ifndef DOS
#include <sys/ioctl.h>
#endif /* DOS */
#ifdef sun
#include <sun/fbio.h>
#endif /* sun */
#include <fcntl.h>
#include <errno.h>

#ifdef XWINDOW
#ifndef HPUX
#include <strings.h>
#endif /* HPUX */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "XVersion.h"
#define LDEX "ldex"
extern char *getenv();
#endif /* XWINDOW */

#define LDEMONO "ldesingle"
#define LDECOLOR "ldemulti"
#define LDETRUECOLOR "ldetruecolor"

#define FBTYPE_SUNFAST_COLOR 12

char filetorun[30];



/************************************************************************/
/*									*/
/*				m a i n					*/
/*									*/
/*	Kick-start program for the "Lisp Development Environment" (lde)	*/
/*									*/
/************************************************************************/

main(argc, argv)
  int argc; char **argv;
  {
    char	Earg[30], Ename[30], **newargv;
    int i;
#ifdef sun
    struct fbtype my_screen;
#endif /* sun */

    int FrameBufferFd;

#ifdef sun
    struct	fbinfo FB_info;
    struct fbgattr FBattr;
#endif /* sun */


  /* Kickstart program for the Lisp Development Environment (LDE).
     Display Device       emulator
     CG3, CG6             lde.multi
     BW2, CG2, CG4, CG9   lde.single
     
     FB-TYPE       REAL-TYPE
     BW2      2             x
     CG2      3             3
     CG3      8             6
     CG4      2             8 
     CG6      8             12
     CG8      6             7
     CG9(GP1) 4             4    ;gpconfig -f -b
     CG9(GP1) 2            13    ;gpconfig gpone0 -f -b cgtwo0 
     ;We assume This config for GXP model
     */

#ifdef XWINDOW
    /* If X-Server exists on the host specified in -display option
       or environment variable DISPLAY, ldex is started. Othewise 
       ldesingle or ldemulti.
       */
      { 
	char *Display_Name = (char *)NULL ;
	Display *Xdisplay = (Display *)NULL;
	int i , option;
	char *pos;

	for( i=1; i<argc; i++ )
	  {

#ifdef XV11R1
	    pos = index( argv[i], ':' );
	    if(pos != NULL)
	      {
		Display_Name = argv[i];
		continue;
	      }
#endif /* XV11R1 */
#ifndef XV11R1
	    if( (strcmp(argv[i], "-d") == 0 )
	        || (strcmp(argv[i], "-display") == 0 ) )
	      {
		if (i == argc) break;
		pos = index( argv[++i], ':' );
		if(pos != NULL)
		  {
		    Display_Name = argv[i];
		  }
		continue;
	      }
#endif /* XV11R1 */
	  }				/*end for() */

	if( (Xdisplay = XOpenDisplay( Display_Name )) != (Display *)NULL )
	  {
	    /* success to connect X-server */
	    XCloseDisplay( Xdisplay );
	    strcpy(filetorun, LDEX);

#ifdef FORKCOMM
/* JRB - call fork_Unix here, while we're REALLY small, unless -NF is
	specified, of course... */
	    for(i=0; i<argc; i++)
		if(!strcmp(argv[i], "-NF")) break;
	    if(i == argc)  /* -NF not in arguments */
		fork_Unix();
#endif /* FORKCOMM */

	    argv[0] = filetorun;
	    execvp(filetorun, argv);
	    perror(filetorun);
	    exit(1);
	  }
	else
	  {			/* failed to connect X-server */
#define NAME_LEN 100
	    char host_name[NAME_LEN];
	    gethostname( host_name, NAME_LEN );
	    if( Display_Name == NULL )
	      {
		if( (Display_Name = getenv("DISPLAY")) != NULL )
		  {
		    if( strncmp( Display_Name
		                 , host_name
		               , strlen( host_name )) == 0)
		      {
			fprintf( stderr, "ldeboot: can't find X-Server\n" );
			exit( -1 );
		      }			/* end if */
		  }
	/* end if */
     	      }
	    else
	      {
		fprintf( stderr, "ldeboot: can't find X-Server\n" );
		exit( -1 );
	      }				/* end if */
	  }				/* end if */
      }
#endif /* XWINDOW */

#ifdef sun
    if( (FrameBufferFd = open( "/dev/fb", O_RDWR )) < 0)
      {
	fprintf( stderr, "ldeboot: can't open FrameBuffer\n");
	exit( -1 );
      }
    if (ioctl(FrameBufferFd, FBIOGTYPE, &my_screen) <0)
      {
	perror("initdisplay0:");
	exit( -1 );
      }

    if( my_screen.fb_type == FBTYPE_SUN4COLOR )
      { /*  cg3 or cg6 */
	if(ioctl(FrameBufferFd,FBIOGATTR,&FBattr) >= 0)
	  {
	    if( FBattr.real_type == FBTYPE_SUN3COLOR || /* cg3 */
		FBattr.real_type == FBTYPE_SUNFAST_COLOR ) /* cg6 */
	      {
		strcpy(filetorun, LDECOLOR);
	      }
  	  }
	else
	  {			/* if( ioctl... */
	    perror("lde: This Display Model does not supported\n");
	    exit( -1 );
	  }
      }
    else if( my_screen.fb_type == FBTYPE_SUN2BW  )
      { /* bw2, cg4 or cg9  */
	strcpy(filetorun, LDEMONO);
      }
    else if( my_screen.fb_type == FBTYPE_SUN3COLOR )
	{ 
	if(ioctl(FrameBufferFd,FBIOGATTR,&FBattr) >= 0)
	  {
	    if( FBattr.real_type == FBTYPE_MEMCOLOR ) /* cg8 */
	      {
		strcpy(filetorun, LDETRUECOLOR);
	      }
  	  }
	else
	  {			/* if( ioctl... */
	    perror("lde: This Display Model does not supported\n");
	    exit( -1 );
	  }
		
	}
    else if( my_screen.fb_type == FBTYPE_SUN2COLOR  )
      { /* cg2  */
	strcpy(filetorun, LDEMONO);
      }
    else
      { 
	perror("lde: This Display Model does not supported\n");
	exit (-1);
      };				/* endif( my_screen... */

    close(FrameBufferFd);

#endif /* sun */



#ifdef FORKCOMM
/* JRB - call fork_Unix here, while we're REALLY small, unless -NF is
	specified, of course... */
   for(i=0; i<argc; i++)
	if(!strcmp(argv[i], "-NF")) break;
   if(i == argc)  /* -NF not in arguments */
	fork_Unix();
#endif



    /* start ldemono or ldecolor */

    argv[0] = filetorun;		/* or whatever... */


    /* then execve the LDE executable */
    execvp(filetorun, argv);
    perror(filetorun);

    exit(1);
  }

