/* @(#) unixcomm.c Version 2.45 (4/21/92). copyright venue & Fuji Xerox  */
static char *id = "@(#) unixcomm.c	2.45 4/21/92	(venue & Fuji Xerox)";
/*

Unix Interface Communications

*/




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

#ifndef DOS

#include "lispemul.h"
#include "address.h"
#include "adr68k.h"
#include "lsptypes.h"
#include "lispmap.h"
#include "emlglob.h"
#include "lspglob.h"
#include "cell.h"
#include "stack.h"
#include "arith.h"
/* JRB - timeout.h needs setjmp.h */
#include <sys/ioctl.h>
#include <setjmp.h>
#include "timeout.h"
#include <stdio.h>
#ifdef OS4
#include <sys/termios.h>
#else
#include <sys/termio.h>
#endif /* OS4 */

#include <sys/types.h>
#ifndef DOS
#include <sys/file.h>
#endif /* DOS */
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include "dbprint.h"
#include <sys/socket.h>
#ifdef ISC
#include <sys/fcntl.h>
#else
#include <sys/un.h>
#endif /* ISC */

#ifdef HPUX
#include <unistd.h>
#endif /* HPUX */


#include "locfile.h"		/* for LispStringToCString. */

char *malloc();

int NPROCS = 100;


/* The following globals are used to communicate between Unix
   subprocesses and LISP */


/* One of these structures exists for every possible file descriptor */
/* type field encodes kind of stream:                                */

#define UJUNUSED    0          /* Unused */
#define UJSHELL    -1          /* PTY shell */
#define UJPROCESS  -2          /* random process */
#define UJSOCKET   -3          /* socket open for connections */
#define UJSOSTREAM -4          /* connection from a UJSOCKET */

struct unixjob {
  char *pathname;               /* used by Lisp direct socket access subr */
  int sockdesc;                 /* descriptor of socket for this process */
  int PID;			/* process ID associated with this slot */
  int status;			/* status returned by subprocess (not shell) */
  int type;
};

struct unixjob *UJ;             /* allocated at run time */

long StartTime;			/* Time, for creating pipe filenames */

#define valid_slot(slot) ((slot) >= 0 && (slot) < NPROCS && UJ[slot].type)

char shcom[2048];		/* Here because I'm suspicious of */
				/* large allocations on the stack */

 

/************************************************************************/
/*									*/
/*		f i n d _ p r o c e s s _ s l o t			*/
/*									*/
/*	Find the slot in UJ with process id 'pid'.		        */
/*	Returns the slot #, or -1 if pid isn't found                    */
/*									*/
/*									*/
/************************************************************************/

int find_process_slot(pid)
/* Find a slot with the specified pid */
  register int pid;
  {
    register int slot;

    for (slot = 0; slot < NPROCS; slot++)
      if (UJ[slot].PID == pid)
	{
	  DBPRINT(("find_process_slot = %d.\n", slot));
	  return slot;
	}
    return -1;
  }



/************************************************************************/
/*									*/
/*		w a i t _ f o r _ c o m m _ p r o c e s s e s		*/
/*									*/
/*									*/
/*									*/
/*									*/
/************************************************************************/

wait_for_comm_processes()
{
  int pid;
  int slot;
  unsigned char d[5];

  d[0] = 'W';
  write(UnixPipeOut, d, 4);
  read(UnixPipeIn, d, 4);
  pid = (d[0]<<8) | d[1];
  while ((pid != 0) && (pid != 65535))
    {
      slot = find_process_slot(pid);
      /* Ignore processes that we didn't start (shouldn't happen but
         occasionally does) */
      if (slot >= 0)
	{
	  if (d[2] == 0)
	    {
	      DBPRINT(("Process %d exited status %d\n", pid, d[3]));
	      UJ[slot].status = d[3];
	    }
	  else
	    {
	      DBPRINT(("Process %d terminated with signal %d\n", pid, d[2]));
	      UJ[slot].status = (d[2] << 8);
            }
	}
      /* Look for another stopped process. */
      d[0] = 'W';
      write(UnixPipeOut, d, 4);
      read(UnixPipeIn, d,4);
      pid = (d[0]<<8) | d[1];
    }
}

/************************************************************************/
/*									*/
/*		b u i l d _ s o c k e t _ p a t h n a m e               */
/*									*/
/*	Returns a string which is the pathname associated with a        */
/*       socket descriptor.  Has ONE string buffer.                     */
/************************************************************************/

char *build_socket_pathname(desc)
int desc;
{
  static char PathName[40];

  sprintf(PathName, "/tmp/LispPipe%d-%d", StartTime, desc);
  return(PathName);
}


/************************************************************************/
/*									*/
/*		c l o s e _ u n i x _ d e s c r i p t o r s             */
/*									*/
/*	Kill off forked PTY-shells and forked-command processes		*/
/*	Also close sockets						*/
/*									*/
/************************************************************************/


close_unix_descriptors() /* Get ready to shut Maiko down */
{
  int slot;

  for (slot = 0; slot < NPROCS; slot++) {

    /* If this slot has an active job */
    switch(UJ[slot].type) {

    case UJSHELL:
      if (kill(UJ[slot].PID, SIGKILL)<0) perror("Killing shell");
      UJ[slot].PID = 0;
      DBPRINT(("Kill 5 closing shell desc %d.\n", slot));
      close(slot);
      break;

    case UJPROCESS:
      if (kill(UJ[slot].PID, SIGKILL)<0) perror("Killing process");
      UJ[slot].PID = 0;
      DBPRINT(("Kill 5 closing process desc %d.\n", slot));
      close(slot);
      break;

    case UJSOCKET:
      close(slot);
      if(UJ[slot].pathname != NULL) {
	/* socket created directly from Lisp; pathname is in .pathname */
	DBPRINT(("Closing socket %d bound to %s\n", slot, UJ[slot].pathname));
	unlink(UJ[slot].pathname);
	free(UJ[slot].pathname);
	UJ[slot].pathname = NULL;
      }
      break;

    case UJSOSTREAM:
      close(slot);
      break;
    }
    UJ[slot].type = UJUNUSED;
  }

/* make sure everyone's really dead before proceeding */
  wait_for_comm_processes();
}

/************************************************************************/
/*								        */
/*			F i n d U n i x P i p e s		        */
/*								        */
/*   Find the file descriptors of the UnixPipe{In,Out} pipes	        */
/*    and a few other important numbers that were set originally        */
/*    before the unixcomm process was forked off; it stuck them in the  */
/*    environment so we could find them after the original lde process  */
/*    got overlaid with the real emulator			        */
/*                                                                      */
/************************************************************************/

FindUnixPipes()
{
  char *envtmp, *getenv();
  register int i;
  struct unixjob cleareduj;

  DBPRINT(("Entering FindUnixPipes\n"));
  UnixPipeIn = UnixPipeOut = StartTime = UnixPID = -1;
  if(envtmp = getenv("LDEPIPEIN"))
    UnixPipeIn = atoi(envtmp);
  if(envtmp = getenv("LDEPIPEOUT"))
    UnixPipeOut = atoi(envtmp);
  if(envtmp = getenv("LDESTARTTIME"))
    StartTime = atoi(envtmp);
  if(envtmp = getenv("LDEUNIXPID"))
    UnixPID = atoi(envtmp);

  /* This is a good place to initialize stuff like the UJ table */
#ifdef HPUX
  NPROCS = sysconf(_SC_OPEN_MAX);
#else
  NPROCS = getdtablesize();
#endif /* HPUX */

  UJ = (struct unixjob *) malloc(NPROCS * sizeof(struct unixjob));
  cleareduj.status = -1;
  cleareduj.pathname = NULL;
  cleareduj.PID = 0;
  cleareduj.type = UJUNUSED;
  for(i=0; i< NPROCS; i++) UJ[i] = cleareduj;

  DBPRINT(("NPROCS is %d; leaving FindUnixPipes\n", NPROCS));
  return (UnixPipeIn == -1
	  || UnixPipeOut == -1
	  || StartTime == -1
	  || UnixPID == -1);
}





/************************************************************************/
/*									*/
/*		    F i n d A v a i l a b l e P t y			*/
/*									*/
/*	Given strings Master and Slave, fill them with path names	*/
/*	of the forms:							*/
/*									*/
/*		Master:  /dev/ptyxx					*/
/*		Slave:   /dev/ttyxx					*/
/*									*/
/*	Which are the first available pty/tty pair for communicating	*/
/*	with a forked shell.						*/
/*									*/
/*	Assumes that valid PTY names are [pqr][0-f]; if your system	*/
/*	is different, you'll need to change it.				*/
/*									*/
/************************************************************************/


#define PTYLETTERS "pqr"
#define PTYNUMBERS "0123456789abcdef"

/* Find the first PTY pair that is not in use */

FindAvailablePty(Master, Slave)
char *Master, *Slave;
{
  int res, flags;
  char *let, *num;

  /* From p to r */
  for (let = PTYLETTERS; *let != 0; let++)

    /* and 0 to f */
    for (num = PTYNUMBERS; *num != 0; num++) {

      sprintf(Master, "/dev/pty%c%c", *let, *num);
      sprintf(Slave, "%c%c", *let, *num);
      DBPRINT(("Trying %s. ", Master));
      /* Try to open the Master side */
      res = open(Master, O_RDWR);

      if (res != -1)
	{
	  flags = fcntl(res, F_GETFL, 0);
	  flags |= FNDELAY;
	  flags = fcntl(res, F_SETFL, flags);
	  return(res);
	}
    }
  return(-1);
}







/************************************************************************/
/*									*/
/*		      U n i x _ h a n d l e c o m m			*/
/*									*/
/*	LISP subr to talk to the forked "Unix process".			*/
/*									*/
/*	The first argument (Arg[0]) is the command number.		*/
/*	Second argument (Arg[1]) is the Job # (except as indicated).	*/
/*									*/
/*	Commands are:							*/
/*									*/
/*		0 Fork Pipe, Arg1 is a string for system();		*/
/*		     => Job # or NIL					*/
/*		1 Write Byte, Arg2 is Byte;				*/
/*		     => 1 (success), NIL (fail)				*/
/*		2 Read Byte => Byte, NIL (no data), or T (EOF)		*/
/*		3 Kill Job => Status or T				*/
/*		4 Fork PTY to Shell (no args) => Job # or NIL		*/
/*		5 Kill All (no args) => T				*/
/*		6 Close (EOF)						*/
/*		7 Job status => T or status				*/
/*		8 => the largest supported command # (11 now)		*/
/*		9 Read Buffer, Arg1 = vmempage (512 byte buffer)	*/
/*		     => byte count (<= 512), NIL (no data), or T (EOF)	*/
/*	       10 Set Window Size, Arg2 = rows, Arg3 = columns		*/
/*	       11 Fork PTY to Shell (obsoletes command 4)		*/
/*		     Arg1 = termtype, Arg2 = csh command string		*/
/*		     => Job # or NIL					*/
/*             12 Create Unix Socket                                    */
/*                   Arg1 = pathname to bind socket to (string)         */
/*                   => Socket # or NIL                                 */
/*             13 Try to accept on unix socket                          */
/*                   => Accepted socket #, NIL (fail) or T (try again)  */
/*             14 Query job type                                        */
/*                   => type number or NIL if not a job                 */
/*             15 Write Buffer, Arg1 = Job #, Arg2 = vmempage,          */
/*                  Arg3 = # of bytes to write from buffer              */
/*                   => # of bytes written or NIL (failed)              */
/*									*/
/************************************************************************/


Unix_handlecomm(args)
  LispPTR *args;
  {
#ifndef ISC
    int command,c, dest, PID, i, slot;
    LispPTR retval;
    unsigned char d[4], b[1];
    unsigned char ch;
    unsigned char buf[1];


    /* Get command */
    N_GETNUMBER(args[0], command, bad);
    DBPRINT(("Unix_handlecomm: trying %d\n", command));

    switch(command)
      {

	case 0:			/* Fork pipe process */
	  {
	    char *PipeName;
	    int res, slot, PipeFD, sockFD;
	    struct sockaddr_un sock;

	    /* First create the socket */
	    sockFD = socket(AF_UNIX, SOCK_STREAM, 0);
	    if (sockFD < 0) { perror("socket open"); return(NIL);}

	    /* then bind it to a canonical pathname */
	    PipeName = build_socket_pathname(sockFD);
	    sock.sun_family = AF_UNIX;
	    strcpy(sock.sun_path, PipeName);
	    if (bind(sockFD, (struct sockaddr *) &sock,
		     strlen(PipeName)+sizeof(sock.sun_family)) < 0)
	      {
		close(sockFD);
		perror("binding sockets");
		return(NIL);
	      }

	    DBPRINT(("Socket %d bound to name %s.\n", sockFD, PipeName));

	    if(listen(sockFD, 1)<0) perror("Listen");

	    d[0] = 'F';
	    d[3] = sockFD;
	    write(UnixPipeOut, d, 4);
	    WriteLispStringToPipe(args[1]);

	    DBPRINT(("Sending cmd string: %s\n", shcom));

	    /* Get status */
	    read(UnixPipeIn, d, 4);

	    /* If it worked, return job # */
	    if (d[3] == 1)
	      {
	      case0_lp:
		TIMEOUT(PipeFD = accept(sockFD, (struct sockaddr *) 0,
					(int *) 0));
		if (PipeFD < 0)
		  {
		    if (errno == EINTR) goto case0_lp;
		    perror("Accept.");
		    close(sockFD);
		    if (unlink(PipeName)<0) perror("Unlink");
		    return(NIL);
		  }

		res = fcntl(PipeFD, F_GETFL, 0);
		res |= FNDELAY;
		res = fcntl(PipeFD, F_SETFL, res);

		UJ[PipeFD].type = UJPROCESS;
		UJ[PipeFD].status = -1;
		UJ[PipeFD].PID = (d[1]<<8) | d[2];
		close(sockFD);
		unlink(PipeName);

		return(GetSmallp(PipeFD));
	      }
	    else
	      {
		DBPRINT(("Fork request failed."));
		close(sockFD);
		unlink(PipeName);
		return(NIL);
	      }
	    break;
	  }

	case 1:			/* Write byte */
	  /* Get job #, Byte */
	  N_GETNUMBER(args[1], slot, bad);
	  N_GETNUMBER(args[2], dest, bad); ch = dest; /* ch is a char */

	  if (valid_slot(slot) && (UJ[slot].status == -1))
	    switch(UJ[slot].type) {

	    case UJPROCESS:
	    case UJSHELL:
	    case UJSOSTREAM:
	      dest = write(slot, &ch, 1);
	      if (dest == 0) {
		wait_for_comm_processes();
		return(NIL);
	      }
	        else return(GetSmallp(1));
	      break;

	    default:  return(NIL);
	    }
	  break;

	case 2:			/* Read byte */
	  /**********************************************************/
	  /* 							    */
	  /* NB that it is possible for the other end of the stream */
	  /* to have terminated, and hence status != -1.	    */
	  /* EVEN IF THERE ARE STILL CHARACTERS TO READ.	    */
	  /* 							    */
	  /**********************************************************/

	  N_GETNUMBER(args[1], slot, bad);	/* Get job # */

	  if (!valid_slot(slot)) return(NIL); /* No fd open; punt the read */

	  switch(UJ[slot].type) {

	  case UJPROCESS:
	  case UJSHELL:
	  case UJSOSTREAM:
	    TIMEOUT(dest = read(slot, buf, 1));
	    if (dest > 0) return(GetSmallp(buf[0]));
		/* Something's amiss; check our process status */
	    wait_for_comm_processes();
	    if ((dest == 0) && (UJ[slot].status == -1))
	      { /* No available chars, but other guy still running */
		return(ATOM_T);
	      }
	    if ((UJ[slot].status == -1)  &&
		((errno == EWOULDBLOCK) || (errno == EAGAIN)) )
	      { /* No available chars, but other guy still running */
		return(ATOM_T);
	      }
	    /* At this point, we either got an I/O error, or there */
	    /* were no chars available and the other end has terminated. */
	    /* Either way, signal EOF. */
	    DBPRINT(("Indicating EOF from PTY desc %d.\n", slot));
	    return(NIL);
	    break;

	  default:
	    return(NIL);
	    break;
	  }
	  break;


	case 3:	  /* Kill process */
		  /* Maiko uses this as CLOSEF, so "process" is a misnomer */

	  N_GETNUMBER(args[1], slot, bad);

	  if (valid_slot(slot))
	    switch(UJ[slot].type)
	      {

		case UJSHELL:
		case UJPROCESS:
			/* First check to see it hasn't already died */
			if (UJ[slot].status == -1)
			  {
			    int i;
			    /* Kill the job */
			    kill(UJ[slot].PID, SIGKILL);
			    for (i = 0; i < 10; i++)
		 	      {
		      /* Waiting for the process to exit is possibly risky.
		         Sending SIGKILL is always supposed to kill
		         a process, but on very rare occurrences this doesn't
		         happen because of a Unix kernel bug, usually a user-
		         written device driver which hasn't been fully
		         debugged.  So we time it out just be safe. */
			      if (UJ[slot].status != -1) break;
			       wait_for_comm_processes();
			      sleep(1);
			    }
			}
	          break;

	        }
	    else return(ATOM_T);

	  switch(UJ[slot].type) {

	  case UJSHELL:
	    DBPRINT(("Kill 3 closing shell desc %d.\n", slot));
	    close(slot);
	    break;

	  case UJPROCESS:
	    DBPRINT(("Kill 3 closing process desc %d.\n", slot));
	    close(slot);
	    break;

	  case UJSOSTREAM:
	    DBPRINT(("Kill 3 closing stream socket desc %d.\n", slot));
	    close(slot);
    	break;

	  case UJSOCKET:
	    DBPRINT(("Kill 3 closing raw socket desc %d.\n", slot));
	    close(slot);
		DBPRINT(("Unlinking %s\n", UJ[slot].pathname));
		if(UJ[slot].pathname) {
			if(unlink(UJ[slot].pathname) < 0)
				perror("Kill 3 unlink");
			free(UJ[slot].pathname);
			UJ[slot].pathname = NULL;
		}
	    break;
	  }
	  UJ[slot].type = UJUNUSED;
	  UJ[slot].PID = 0;
	  UJ[slot].pathname = NULL;

	  /* If status available, return it, otherwise T */
	  return(GetSmallp(UJ[slot].status));
	  break;


	case 4:	case 11:		/* Fork PTY process */
	  {
	    char MasterFD[20], SlavePTY[4];
	    int Master, res, slot;

	    Master = FindAvailablePty(MasterFD, SlavePTY);
	    slot = Master;
	    DBPRINT(("Fork Shell; Master PTY = %d. Slave=%c%c.\n",
		     Master, SlavePTY[0], SlavePTY[1]));
	    if (Master < 0)
	      {
		printf("Open of lisp side of PTY failed.\n");
		fflush(stdout);
		return(NIL);
	      }

	    d[0] = (command == 4) ? 'S': 'P';
	    d[1] = SlavePTY[0];
	    d[2] = SlavePTY[1];
	    d[3] = slot;
	    write(UnixPipeOut, d, 4);

	    if (command != 4)
	      {	/* New style has arg1 = termtype, arg2 = command */
		WriteLispStringToPipe (args[1]);
		WriteLispStringToPipe (args[2]);
	      }

	    /* Get status */
	    read(UnixPipeIn, d, 4);
	    /* If successful, return job # */
	    DBPRINT(("Pipe/fork result = %d.\n", d[3]));
	    if (d[3] == 1)
	      {
		    /* Set up the IO not to block */
		res = fcntl(Master, F_GETFL, 0);
		res |= FNDELAY;
		res = fcntl(Master, F_SETFL, res);

		UJ[slot].type = UJSHELL; /* so we can find them */
		UJ[slot].PID = (d[1]<<8) | d[2];
		UJ[slot].status = -1;

		return(GetSmallp(slot));
	      }
	     else
	      {
		printf("Fork failed.\n"); fflush(stdout);
		close(Master);
		return(NIL);
	      }
	    break;
	  }

	case 5:			/* Kill all the subprocesses */

	  close_unix_descriptors();
	  return(ATOM_T);

	case 6:			/* Kill this subprocess */
	  d[0] = 'C';

	  /* Get job # */
	  N_GETNUMBER(args[1], dest, bad);
	  d[1] = dest;

	  d[3] = 1;
	  write(UnixPipeOut, d, 4);

	  /* Get status */
	  read(UnixPipeIn, d, 4);
	  switch(UJ[dest].type) {

	  case UJSHELL:
	    DBPRINT(("Kill 5 closing shell desc %d.\n", dest));
	    close(dest);
	    break;

	  case UJPROCESS:
	    DBPRINT(("Kill 5 closing process desc %d.\n", dest));
	    close(dest);
	    break;

	  case UJSOCKET:
	    /* close a socket; be sure and unlink the file handle */
	    DBPRINT(("Kill 5 closing raw socket desc %d.\n", dest));
		close(dest);
	    if(UJ[dest].pathname != NULL) {
	      unlink(UJ[dest].pathname);
	      free(UJ[dest].pathname);
	      UJ[dest].pathname = NULL;
	    } /* else return an error somehow... */
	    break;

	  case UJSOSTREAM:
	    DBPRINT(("Kill 5 closing socket stream %d.\n", dest));
	    close(dest);
	    break;
	  }

	  UJ[dest].type = UJUNUSED;
	  UJ[dest].PID = 0;
	  return(ATOM_T);
       /* break; */


	case 7:			/* Current job status */

	  N_GETNUMBER(args[1], slot, bad);	  /* Get job # */
	  wait_for_comm_processes();   /* Make sure we're up to date */

	  if (UJ[slot].status == -1) return (ATOM_T);
	  else return(GetSmallp(UJ[slot].status));
	  break;

	case 8:			/* Return largest supported command */

	  return(GetSmallp(15));
	  

	case 9:			/* Read buffer */
	  /**********************************************************/
	  /* 							    */
	  /* NB that it is possible for the other end of the stream */
	  /* to have terminated, and hence ForkedStatus != -1.	    */
	  /* EVEN IF THERE ARE STILL CHARACTERS TO READ.	    */
	  /* 							    */
	  /**********************************************************/

	  { char *bufp;
	    int terno;    /* holds errno thru sys calls after I/O fails */

	    N_GETNUMBER(args[1], slot, bad); 		/* Get job # */
	    if (!valid_slot(slot)) return(NIL); /* No fd open; punt the read */

	    bufp = (char*)(Addr68k_from_LADDR(args[2]));  /* User buffer */
	    DBPRINT(("Read buffer, type is %d\n", UJ[slot].type));

	    switch(UJ[slot].type) {
	    case UJSHELL:
	    case UJPROCESS:
	    case UJSOSTREAM:
	      dest = read(slot, bufp, 512);
#ifdef BYTESWAP
	      word_swap_page(bufp, 128);
#endif /* BYTESWAP */

	      if (dest > 0) return(GetSmallp(dest));
		  /* Something's amiss; update process status */
	      wait_for_comm_processes();  /* make sure we're up to date */
	      if (((dest == 0) || (errno == EINTR) || (errno == 0) ||
		   (errno == EAGAIN) ||
		   (errno == EWOULDBLOCK)) && (UJ[slot].status == -1))
		/* No available chars, but other guy still running */
		return(ATOM_T);
	      /* At this point, we either got an I/O error, or there */
	      /* were no chars available and the other end has terminated. */
	      /* Either way, signal EOF. */
	      DBPRINT(("read failed; dest = %d, errno = %d, status = %d\n",
		       dest, terno, UJ[slot].status));
	      DBPRINT(("Indicating EOF from PTY desc %d.\n", slot));
	      return(NIL);

	    default:
	      return(NIL);
	    }
	  }

	case 10:			/* Change window */
	  { int rows, cols, pgrp, pty;
#if (!defined( HPUX) && !defined(RISCOS))
	    struct winsize w;
#endif /* HPUX */

				/* Get job #, rows, columns */
	    N_GETNUMBER(args[1], slot, bad);
	    N_GETNUMBER(args[2], rows, bad);
	    N_GETNUMBER(args[3], cols, bad);

#if (!defined(HPUX) && !defined(RISCOS))
	    if (valid_slot(slot)
	        && (UJ[slot].type == UJSHELL)
	        && (UJ[slot].status == -1))
	      {
	        w.ws_row = rows;
	        w.ws_col = cols;
	        w.ws_xpixel = 0;	/* not used */
	        w.ws_ypixel = 0;
	        pty = slot;
				/* Change window size, then
				   notify process group of the change */
	        if ((ioctl(pty, TIOCSWINSZ, &w) >= 0) &&
		    (ioctl(pty, TIOCGPGRP, &pgrp) >= 0) &&
#ifdef RISCOS
		    (kill(-pgrp, SIGWINCH) >= 0))
#else
		    (killpg(pgrp, SIGWINCH) >= 0))
#endif /* RISCOS */

		  return (ATOM_T);
	        return(GetSmallp(errno));
	      }
#endif /* HPUX | RISCOS */
	    return(NIL);
	  }


	case 12:			/* create Unix socket */

	  {
	    int res, sockFD;
	    struct sockaddr_un sock;


	    /* First open the socket */
	    sockFD = socket(AF_UNIX, SOCK_STREAM, 0);
	    if (sockFD < 0)
	      { perror("socket open");
		return(NIL);}
	    /* Then get a process slot and blit the pathname of the
	       socket into it */
	    /* need to type-check the string here */
	    LispStringToCString(args[1], shcom, 2048);
	    UJ[sockFD].pathname = malloc(strlen(shcom) + 1);
	    strcpy(UJ[sockFD].pathname, shcom);
	    /* Then bind it to the pathname, and get it	listening properly */

	    sock.sun_family = AF_UNIX;
	    strcpy(sock.sun_path, shcom);
	    if (bind(sockFD, (struct sockaddr *) &sock,
		     strlen(shcom)+sizeof(sock.sun_family)) < 0)
	      {
		close(sockFD);
		free(UJ[sockFD].pathname);
		UJ[sockFD].type = UJUNUSED;
		perror("binding Lisp sockets");
		return(NIL);
	      }
	    DBPRINT(("Socket %d bound to name %s.\n", sockFD, shcom));
	    if(listen(sockFD, 1)<0) perror("Listen");
		    /* Set up the IO not to block */
	    res = fcntl(sockFD, F_GETFL, 0);
	    res |= FNDELAY;
	    res = fcntl(sockFD, F_SETFL, res);

	    /* things seem sane, fill out the rest of the UJ slot and return */
	    UJ[sockFD].status = -1;
	    UJ[sockFD].PID = -1;
	    UJ[sockFD].type = UJSOCKET;

	    return(GetSmallp(sockFD));
	}
	    break;


	case 13:  /* try to accept */
		{
		  /* returns file descriptor if successful,
		     -1 if no connection available,
		     NIL if failure */
		  int sockFD, newFD;

		  N_GETNUMBER(args[1], sockFD, bad);
		  if(UJ[sockFD].type == UJSOCKET &&
		     UJ[sockFD].pathname != NULL) {
		    /* sockFD SHOULD be non-blocking;
		       but I'll time this out just in case */
		case13_lp:
		    TIMEOUT(newFD = accept(sockFD, (struct sockaddr *) 0,
					 (int *) 0));
		    if (newFD < 0)
		      if (errno == EINTR) goto case13_lp;
		      else if (errno == EWOULDBLOCK)
			return (GetSmallp(-1));
		      else {
			perror("Lisp socket accept");
			return (NIL);
		      }
		    else {
		      UJ[newFD].status = -1;
		      UJ[newFD].PID = -1;
		      UJ[newFD].type = UJSOSTREAM;
		      return(GetSmallp(newFD));
		    }
		  } else return(NIL);
		}
	  break;


	case 14:  /* return type of socket */
		{
		 int streamFD;

		 N_GETNUMBER(args[1], streamFD, bad);
		 if(valid_slot(streamFD))
			return GetSmallp(UJ[streamFD].type);
		 else return NIL;
		}
		break;


	
	case 15:			/* Write buffer */
	  { char *bufp;
	    N_GETNUMBER(args[1], slot, bad); 		    /* Get job # */
	    bufp = (char*)(Addr68k_from_LADDR(args[2]));    /* User buffer */
	    N_GETNUMBER(args[3], i, bad);                   /* # to write */
	    DBPRINT(("Write buffer, type is %d\n", UJ[slot].type));

	    switch(UJ[slot].type) {
	    case UJSHELL:
	    case UJPROCESS:
	    case UJSOSTREAM:
#ifdef BYTESWAP
	      word_swap_page(bufp, (i+3)>>2);
#endif /* BYTESWAP */

	      dest = write(slot, bufp, i);
#ifdef BYTESWAP
	      word_swap_page(bufp, (i+3)>>2);
#endif /* BYTESWAP */

	      if (dest > 0) return(GetSmallp(dest));
		  /* Something's amiss; update process status */
	      wait_for_comm_processes();  /* make sure we're up to date */
	      if (((dest == 0) ||
		   (errno == EWOULDBLOCK)) && (UJ[slot].status == -1))
		/* No room to write, but other guy still running */
		return(ATOM_T);
	      /* At this point, we either got an I/O error, or there */
	      /* were no chars available and the other end has terminated. */
	      /* Either way, signal EOF. */
	      DBPRINT(("Indicating write failure from PTY desc %d.\n", slot));
	      return(NIL);
	    }
	  }

	default: return(NIL);
	}

      bad:
	DBPRINT(("Bad input value."));
	return(NIL);
#endif /* ISC */

      }




/************************************************************************/
/*									*/
/*		W r i t e L i s p S t r i n g T o P i p e		*/
/*									*/
/*	Convert a lisp string to a C string (both format and byte-	*/
/*	order), write 2 bytes of length and the string			*/
/*									*/
/*									*/
/************************************************************************/

WriteLispStringToPipe (lispstr)
  LispPTR lispstr;
  { unsigned short len;
    LispStringToCString(lispstr, shcom, 2048);
    /* Write string length, then string */
    len = strlen(shcom) + 1;
    write(UnixPipeOut, &len, 2);
    write(UnixPipeOut, shcom, len);
  }

#endif /* DOS */
