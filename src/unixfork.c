/* @(#) unixfork.c Version 1.4 (9/18/90). copyright venue & Fuji Xerox  */
static char *id = "@(#) unixfork.c	1.4 9/18/90	(venue & Fuji Xerox)";


/************************************************************************/
/*									*/
/*		Code to fork a subprocess for Unix communication	*/
/*									*/
/*									*/
/*									*/
/************************************************************************/

/************************************************************************/
/*									*/
/*	(C) Copyright 1989-1992 Venue. All Rights Reserved.		*/
/*	Manufactured in the United States of America.			*/
/*									*/
/*	The contents of this file are proprietary information 		*/
/*	belonging to Venue, and are provided to you under license.	*/
/*	They may not be further distributed or disclosed to third	*/
/*	parties without the specific permission of Venue.		*/
/*									*/
/************************************************************************/

#ifdef OS4
#define USETERMIOS
#endif
#ifdef INDIGO
#define USETERMIOS
#define TCSETS TCSETA
#define TCGETS TCGETA
#endif


#ifndef USETERMIOS
#include <sys/ioctl.h>
#else
#include <sys/termios.h>
#endif USETERMIOS

#ifdef HPUX
#include <sgtty.h>
#include <unistd.h>
#endif HPUX
#ifdef RISCOS
#include <sgtty.h>
#endif

#include <stdio.h>
#include <sys/types.h>
#include <sys/file.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include "dbprint.h"
#include <sys/socket.h>
#ifndef SYSVONLY
#include <sys/un.h>
#endif SYSVONLY
#ifdef RISCOS
#include <sys/un.h>
#endif RISCOS
#ifdef INDIGO
#include <sys/un.h>
#include <sgtty.h>
#endif


/* The following globals are used to communicate between Unix
   subprocesses and LISP */


long StartTime;			/* Time, for creating pipe filenames */

char shcom[512];		/* Here because I'm suspicious of */
				/* large allocations on the stack */

/* fork_Unix is the secondary process spawned right after LISP is
   started, to avoid having TWO 8 mbyte images sitting around. It listens
   to the pipe LispToUnix waiting for requests, and responds on UnixToLisp.
   The data passed through this pipe is in 4 byte packets, of the form:

   Byte 0:   Command character, one of:
                   S: Fork PTY (shell) process. This is used for CHAT windows.
		   P: New version of S, takes 2 string args.
		   F: Fork piped shell, takes 1 string arg.
		   K: Kill process
		   E: Exit (kill all subprocesses)
		   C: Close stdin to subprocess
		   W: call WAIT3 & get one process's close info.
		   O: Fork OCR process.
   Byte 1:   Process number (0 to NPROCS - 1)
             Not used for S, F, and E commands
	     [For S&P, pty letter]
	     [For F, process # for pipe naming]
   Byte 2:   Value, used as follows:
             Only used for W command, contains byte to write
	     [For S&P, pty number]
   Byte 3:   Slot number.

In the case of F & P commands, additional data follows the 4 byte packet.
This consists of 2 bytes representing the length of the shell command
string, and the string itself.

fork_Unix will return another 4 byte packet. The bytes are the same as those
of the packet received except:

   F:        Byte 2 is job number
             Byte 3 is 1 if successful, 0 if not
   S:	     Byte 2 is job number
             Byte 3 is 1 if successful, 0 if not
   R:        Byte 2 is value of byte read from stdin, if any
             Byte 3 is 1 if successful, 2 if EOF, 0 if nothing waiting
   W:        Bytes 0 & 1 are the Process ID of the terminated process
	     Bytes 2 & 3 are the high & low bytes of the exit status.
   K:        Bytes 1 and 2 are the high and low bytes of the exit status
             of the process.
	     Byte 3 is 1 if an exit status was available.
   E:        Always the same
   C:        Always the same
   O:	     Byte 3 is 1 if successful, 0 if not
   	     Byte 1 and Byte 2 are the process ID of OCR process

*/



fork_Unix()
{

  int LispToUnix[2],		/* Incoming pipe from LISP */
      UnixToLisp[2],		/* Outgoing pipe to LISP */
	 UnixPID,
      LispPipeIn, LispPipeOut, res, pid, slot;

  char IOBuf[4];
  unsigned short tmp;
  char *cmdstring;

  /* Pipes between LISP subr and process */
  if (pipe(LispToUnix) == -1) {
    perror("pipe");
    exit(-1);
  }
  if (pipe(UnixToLisp) == -1) {
    perror("pipe");
    exit(-1);
  }

  StartTime = time(0);		/* Save the time, to create filenames with */
  StartTime &= 0xFFFFFF;	/* as a positive number! */

/* interrupts need to be blocked here so subprocess won't see them */
#ifdef SYSVSIGNALS
  sighold(SIGVTALRM);
  sighold(SIGIO);
  sighold(SIGALRM);
  sighold(SIGXFSZ);
  sighold(SIGFPE);
#else
  sigblock(sigmask(SIGVTALRM) | sigmask(SIGIO)
		| sigmask(SIGALRM)
#ifndef HPUX
		| sigmask(SIGXFSZ)
#endif HPUX
 		| sigmask(SIGFPE)
		);
#endif SYSVSIGNALS

  if ((UnixPID = fork()) == -1) {  /* Fork off small version of the emulator */
    perror("fork");
    exit(-1);
  }

  if (UnixPID != 0) {
    /* JRB - fork_Unix is now called in ldeboot; leave UnixPipe{In,Out} open
       and put their numbers in the environment so parent can find them */
    /* JDS - NB that sprintf doesn't always return a string! */

    char* tempstring;

    tempstring = (char *) malloc(30);
    sprintf(tempstring, "LDEPIPEIN=%d", UnixToLisp[0]);
    putenv(tempstring);

    tempstring = (char *) malloc(30);
    sprintf(tempstring, "LDEPIPEOUT=%d", LispToUnix[1]);
    putenv(tempstring);

    tempstring = (char *) malloc(30);
    sprintf(tempstring, "LDESTARTTIME=%d", StartTime);
    putenv(tempstring);

    tempstring = (char *) malloc(30);
    sprintf(tempstring, "LDEUNIXPID=%d", UnixPID);
    putenv(tempstring);

    close(LispToUnix[0]);
    close(UnixToLisp[1]);
    return(1);
  }

  LispPipeIn = LispToUnix[0];
  LispPipeOut = UnixToLisp[1];
  close(LispToUnix[1]);
  close(UnixToLisp[0]);

  res = fcntl(LispPipeIn, F_GETFL, 0);
  res &= (65535-FNDELAY);
  res = fcntl(LispPipeIn, F_SETFL, res);

  while (1) {
    int len;
    len = 0;
    while (len != 4)
    {
      if ((len = read(LispPipeIn, IOBuf, 4)) <0)
	{	/* Get packet */
	  perror("Packet read by slave");
	  /*      kill_comm_processes(); */
	  exit(0);
	}
      if (len != 4)
	{
	  DBPRINT(("Input packet wrong length:  %d.\n", len));
	  exit(0);
	}
    }
    slot = IOBuf[3];
    IOBuf[3] = 1;
    switch(IOBuf[0]) {

    case 'S': case 'P':			/* Fork PTY shell */
      if (slot >= 0)
	{ /* Found a free slot */
	  char termtype[32];
	  if (IOBuf[0] == 'P')
	    {			/* The new style, which takes term type & command to csh */
	      if(read(LispPipeIn, &tmp, 2) < 0) perror("Slave reading cmd length");
	      if(read(LispPipeIn, termtype, tmp) < 0) perror("Slave reading termtype");
	      if(read(LispPipeIn, &tmp, 2) < 0) perror("Slave reading cmd length");
	      if (tmp > 510) cmdstring = (char *)malloc(tmp+5);
	      else cmdstring = shcom;

	      if(read(LispPipeIn, cmdstring, tmp) < 0) perror("Slave reading shcom");
	    }
	    else		/* old style, no args */
	      { termtype[0] = 0; cmdstring[0] = 0; }

	  /* Alloc a PTY and fork  */
	  pid = ForkUnixShell(slot, IOBuf[1], IOBuf[2], termtype, cmdstring);

	  if (pid == -1)
	    {
	      printf("Impossible failure from ForkUnixShell??\n");
	      fflush(stdout);
	      IOBuf[3] = 0;
	    }
	  else
	    {
	      /* ForkUnixShell sets the pid and standard in/out variables */
	      IOBuf[1] = (pid >> 8)& 0xFF;
	      IOBuf[2] = pid & 0xFF;
	    }
        }
      else 
	{
	  printf("Can't get process slot for PTY shell.\n");
	  fflush(stdout);
	  IOBuf[3] = 0;
	}
       break;

    case 'F':			/* Fork pipe command */
      if (slot >= 0) 	
	{

	/* Read in the length of the shell command, and then the command */
	if(read(LispPipeIn, &tmp, 2)<0) perror("Slave reading cmd length");
	if (tmp > 510) cmdstring = (char *) malloc(tmp+5);
	else cmdstring = shcom;
	if(read(LispPipeIn, cmdstring, tmp)<0) perror("Slave reading cmd");
	DBPRINT(("Cmd len = %d.\n", tmp));
	DBPRINT(("Rev'd cmd string: %s\n", cmdstring));
	pid = fork(); /* Fork */

	if (pid == 0)
	  {
	    int i;
	    int status, sock;
	    struct sockaddr_un addr;
	    char PipeName[40];

	    sock = socket(AF_UNIX, SOCK_STREAM, 0);
	    if (sock < 0) {perror("slave socket"); exit(0);}
	    sprintf(PipeName, "/tmp/LispPipe%d-%d", StartTime, slot);
	    addr.sun_family = AF_UNIX;
	    strcpy(addr.sun_path, PipeName);
	    status = connect(sock, (struct sockaddr *)&addr,
			     strlen(PipeName)+sizeof(addr.sun_family));
	    if(status<0)
	      {
		perror("slave connect");
		printf("Name = %s.\n", PipeName); fflush(stdout);
		exit(0);
	      }
	    else
	      {
		DBPRINT(("Slave connected on %s.\n", PipeName));
	      }

	    /* Copy the pipes onto stdin, stdout, and stderr */
	    dup2(sock,0);
	    dup2(sock,1);
	    dup2(sock,2);
 
#ifdef HPUX
	    /* Make sure everything else is closed POSIX has no getdtab... */
	    for (i = 3; i < sysconf(_SC_OPEN_MAX); i++)
	      close(i);
#else
	    /* Make sure everything else is closed */
	    for (i = 3; i < getdtablesize(); i++)
	      close(i);
#endif HPUX

	    /* Run the shell command and get the result */
	    status = system(cmdstring);
	    if (cmdstring != shcom) free(cmdstring);
	    /* Comment out to fix USAR 11302 (FXAR 320)
	    unlink(PipeName);
	    */
	    _exit((status & ~0xff) ? (status >> 8) : status);
	  }

	/* Check for error doing the fork */
	if (pid == -1)
	  {
	    perror("unixcomm: fork");
	    IOBuf[3] = 0;
	  }
	else
	  {
	    IOBuf[1] = (pid >> 8)& 0xFF;
	    IOBuf[2] = pid & 0xFF;

	  }
      } else IOBuf[3] = 0; /* Couldn't get a process slot */
      break;


    case 'W':			/* Wait for a process to die. */
      {
	int pid;
#ifdef RISCOS
	int status;
#else
	union wait status;
#endif RISCOS
	int slot;

#ifdef RISCOS
	status = 0;
#else
	status.w_status = 0;
#endif RISCOS
	IOBuf[0] = 0; IOBuf[1] = 0;
	DBPRINT(("About to wait for processes.\n"));
#ifdef RISCOS
	if (((pid = wait(&status, WNOHANG)) != 0))
#else
	if (((pid = wait3(&status, WNOHANG, 0)) != 0))
#endif RISCOS
	  {
	    /* Ignore processes which are suspended but haven't exited
	       (this shouldn't happen) */
#ifdef RISCOS
	    if ((status & 0xFF) == 0177) break;
	    IOBuf[3] = status >>8;
	    IOBuf[2] = status & 0xFF;
#else
	    if (status.w_stopval == WSTOPPED) break;
	    IOBuf[3] = status.w_T.w_Retcode;
	    IOBuf[2] = status.w_T.w_Termsig;
#endif RISCOS
	    IOBuf[1] = pid & 0xFF;
	    IOBuf[0] = (pid>>8) & 0xFF;
          }
	DBPRINT(("wait3 returned pid = %d.\n", pid));
      }

      break;

    case 'C':			/* Close stdin to subprocess */

      break;


    case 'K':			/* Kill subprocess */

      break;


#ifdef OCR
    case 'w':			/* Wait paticular process to die */
      {
	      int	pid, res, status;

	      pid = IOBuf[1] << 8 | IOBuf[2];

	    retry:
	      res = waitpid(pid, &status, WNOHANG);
	      if (res == -1 && errno == EINTR) goto retry;
	      
	      if (res == pid) {
		      IOBuf[0] = res >> 24 & 0xFF;
		      IOBuf[1] = res >> 16 & 0xFF;
		      IOBuf[2] = res >> 8 & 0xFF;
		      IOBuf[3] = res & 0xFF;
	      } else {
		      IOBuf[0] = IOBuf[1] = IOBuf[2] = IOBuf[3] = 0;
	      }
      }
      break;

    case 'O':			/* Fork OCR process */
      if (slot >= 0) {
	      pid_t	ppid;
	      ppid = getppid();
	      pid = fork();
	      if (pid == 0) {
		      int			i;
		      int			status, len;
		      struct sockaddr_un	addr;
		      char			PipeName[40];
		      extern int		OCR_sv;

		      OCR_sv = socket(AF_UNIX, SOCK_STREAM, 0);
		      if (OCR_sv < 0) {
			      perror("slave socket");
			      exit(0);
		      }
		      sprintf(PipeName, "/tmp/LispPipe%d-%d", StartTime, slot);
		      addr.sun_family = AF_UNIX;
		      strcpy(addr.sun_path, PipeName);
		      len = strlen(PipeName)+sizeof(addr.sun_family);
		      status = connect(OCR_sv, &addr, len);
		      if(status<0) {
			      perror("slave connect");
			      OCR_sv = -1;
			      exit(0);
		      }

		      (void)ocr_proc(ppid);
		      OCR_sv = -1;
		      exit(1);
	      }

	      if (pid == -1) {
		      perror("unixcomm: fork OCR");
		      IOBuf[3] = 0;
	      } else {
		      IOBuf[1] = (pid >> 8)& 0xFF;
		      IOBuf[2] = pid & 0xFF;
	      }
      } else IOBuf[3] = 0;
      break;
#endif OCR

    } /* End of switch */

    /* Return the status/data packet */
    write(LispPipeOut, IOBuf, 4);
  }
}




/************************************************************************/
/*									*/
/*			F o r k U n i x S h e l l			*/
/*									*/
/*	Fork a PTY connection to a C-shell process.			*/
/*	Returns PID of process, or -1 if something failed		*/
/*									*/
/*									*/
/************************************************************************/


/* Creates a PTY connection to a csh */

ForkUnixShell(slot, ltr, numb, termtype, shellarg)
  int slot;
  char ltr, numb;
  char *termtype, *shellarg;

  {
    char PtySlave[20], buf[1];
    int res, PID, MasterFD, SlaveFD;
#ifdef USETERMIOS
    struct termios tio;
#else
    struct sgttyb tio;
#endif USETERMIOS

    PID = fork();

    if (PID == 0)
      {
	char envstring[64];
	char *argvec[4];
	
	/* Divorce ourselves from /dev/tty */
	res = open("/dev/tty", O_RDWR);
	if (res >= 0)
	  {
#if (!defined(HPUX) && !defined(RISCOS))
	    (void) ioctl(res, TIOCNOTTY, (char *)0);
#endif
	    (void) close(res);
	  }
	else
	  {
	    perror("Slave TTY");
	    exit(0);
	  }

	/* Open the slave side */
	sprintf(PtySlave, "/dev/tty%c%c", ltr, numb);
	SlaveFD = open(PtySlave, O_RDWR);
	if (SlaveFD == -1)
	  {
	    perror("Slave Open");
	    exit(0);
	  }


#ifndef USETERMIOS
/* This is the old way we set up terminal (OS 3), using an
   obsolete ioctl and wrong flags for a display. */
	ioctl(SlaveFD, TIOCGETP, (char *)&tio);
	tio.sg_flags |= CRMOD;
	tio.sg_flags |= ECHO;
	ioctl(SlaveFD, TIOCSETP, (char *)&tio);
#else
	/* Set up as basic display terminal: canonical erase,
	   kill processing, echo, backspace to erase, echo ctrl
	   chars as ^x, kill line by backspacing */

	ioctl(SlaveFD, TCGETS, (char *)&tio);
#ifdef INDIGO
	tio.c_lflag |= ICANON | ECHO | ECHOE;
#else
	tio.c_lflag |= ICANON | ECHO | ECHOE | ECHOCTL | ECHOKE;
#endif /* INDIGO */
	ioctl(SlaveFD, TCSETS, (char *)&tio);
#endif USETERMIOS
	(void) dup2(SlaveFD, 0);
	(void) dup2(SlaveFD, 1);
	(void) dup2(SlaveFD, 2);
	(void) close(SlaveFD);

	/* set the LDESHELL variable so the underlying .cshrc can see it and
	   configure the shell appropriately, though this may not be so important any more */
	putenv("LDESHELL=YES");

	if ((termtype[0] != 0) && (strlen(termtype) < 59))
	  { 		/* set the TERM environment var */
	    sprintf(envstring, "TERM=%s", termtype);
	    putenv(envstring);
	  }
	/* Start up csh */
	argvec[0] = "csh";
	if (shellarg[0] != 0)
	  {			/* setup to run command */
	    argvec[1] = "-c";	/* read commands from next arg */
	    argvec[2] = shellarg;
	    argvec[3] = (char *) 0;
	  }
	  else argvec[1] = (char *) 0;

	execv("/bin/csh", argvec);

	/* Should never get here */
	perror("execv");
	exit(0);
      }
    else
      { /* not the forked process. */
	if (shellarg != shcom) free(shellarg);
      }

    /* Set the process group so all the kids get the bullet too
    if (setpgrp(PID, PID) != 0)
      perror("setpgrp"); */

    return(PID);
  }

