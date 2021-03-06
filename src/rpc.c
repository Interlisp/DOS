/* @(#) rpc.c Version 1.16 (4/21/92). copyright venue & Fuji Xerox  */
static char *id = "@(#) rpc.c   1.16 4/21/92    (venue & Fuji Xerox)";



/************************************************************************/
/*                                                                      */
/*      (C) Copyright 1989, 1990, 1991 Venue. All Rights Reserved.      */
/*      Manufactured in the United States of America.                   */
/*                                                                      */
/*      The contents of this file are proprietary information           */
/*      belonging to Venue, and are provided to you under license.      */
/*      They may not be further distributed or disclosed to third       */
/*      parties without the specific permission of Venue.               */
/*                                                                      */
/************************************************************************/
/************************************************************************/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/


#ifndef DOS
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <sys/ioctl.h>
#endif /* DOS */
#include "lispemul.h"
#include "lispmap.h"
#include "lsptypes.h"
#include "lspglob.h"
#include "emlglob.h"
#include "adr68k.h"
#include "arith.h"
#include "locfile.h"

#define MAX_HOSTNAME_LENGTH 100
#define UDP_DATA_BLOCK_SIZE 1000


rpc(args)
  LispPTR *args;
{
#ifndef DOS
  /* Arguments are:
     args[0]:Destination Address; hostname or internet address are both supported.
     args[1]:Remote port for this program.
     args[2]:Argument block pointer.
     args[3]:Result Block pointer.
     args[4]:Milliseconds before timeout
     args[5]:Milliseconds between tries
     args[6]:Argument block length
     */
  char hostname[MAX_HOSTNAME_LENGTH];
  struct hostent *hp;
  struct servent *sp;
  struct sockaddr_in sin, sin1, from;
  char *outbuf, *inbuf, *destaddr;
  register int s, msec_until_timeout, msec_between_tries, out_length;
  register int  received,  mask;
  register int port;
  int dontblock, dest, fromlen, read_descriptors;

  struct timeval pertry_timeout, total_timeout, time_waited;

  /* Set timeout */
  /* CONVERT FROM LISP TO C TYPES */
  dest = GetTypeNumber(args[0]);

  if ((dest == TYPE_FIXP) || (dest == TYPE_SMALLP)) {
    N_GETNUMBER(args[0], dest, handle_error);
    destaddr = (char *)&dest;
    hp = gethostbyaddr(destaddr, sizeof(struct in_addr), AF_INET);
  }
  else {
    /* Convert Hostname */
    LispStringToCString(args[0],hostname, MAX_HOSTNAME_LENGTH);
    hp = gethostbyname(hostname);
  }
    
  N_GETNUMBER(args[1], port, handle_error);

  /* Translate the buffer pointer into C pointers */
  outbuf = (char *)(Addr68k_from_LADDR(args[2]));
  inbuf = (char *)(Addr68k_from_LADDR(args[3]));
                   
  N_GETNUMBER(args[4], msec_until_timeout, handle_error);

  N_GETNUMBER(args[5], msec_between_tries, handle_error);
  
  N_GETNUMBER(args[6], out_length, handle_error);

  /* Convert to micro seconds */
  msec_until_timeout = msec_until_timeout * 1000;
  msec_between_tries = msec_between_tries * 1000;

  /* Set up the timeouts */
  total_timeout.tv_sec = msec_until_timeout / 1000000;
  total_timeout.tv_usec = msec_until_timeout % 1000000;
  pertry_timeout.tv_sec = msec_between_tries / 1000000;
  pertry_timeout.tv_usec = msec_between_tries % 1000000;

  /* SET UP THE SOCKET */

  /* Open the socket; Might want to make this non-blocking */
  if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    goto handle_error;

  dontblock = 1;

  /* The sockets that rpc controls don't block */
#ifdef SYSVONLY
/* NEED TO FILL THIS IN PROPERLY */
#else
 (void)ioctl(s, FIONBIO, &dontblock);
#endif /* SYSVONLY */

  
  bzero((char *)&sin, sizeof(sin));
  sin.sin_family = AF_INET;
  bind(s, &sin, sizeof(sin));

  /* Resolve the host address. */
  if (hp) {
    sin1.sin_family = hp->h_addrtype;
    bcopy(hp->h_addr,(caddr_t) &sin1.sin_addr, hp->h_length);
  }
  else goto handle_error;

  /* Convert to network byte order */
  sin1.sin_port = htons((u_short)port);

  /* Send buffer out on the socket */
  if (sendto(s, outbuf, out_length, 0, (struct sockaddr *)&sin1,
             sizeof(sin1)) != out_length)
    goto handle_error;

  /* Set the select mask */
  mask = 1 << s;
  
  /* Set up the timers */
  time_waited.tv_sec = 0;
  time_waited.tv_usec = 0;
  
  /* Start the waiting loop */
 receive:

  read_descriptors = mask;

  switch(select(32, &read_descriptors, (int *)NULL, (int *)NULL, 
                &pertry_timeout)){
    
    /* Per try timeout expired, Check the total timeout */
  case 0:
    time_waited.tv_sec += pertry_timeout.tv_sec;
    time_waited.tv_usec += pertry_timeout.tv_usec;
    while(time_waited.tv_usec >= 1000000) {
      time_waited.tv_sec++;
      time_waited.tv_usec -= 1000000;
    }
    /* If the time waited is greater than the total 
     * timeout then there's an error
     */
    if ((time_waited.tv_sec > total_timeout.tv_sec) ||
        ((time_waited.tv_sec == total_timeout.tv_sec) &&
         (time_waited.tv_usec >= total_timeout.tv_usec)))
      goto handle_error;
    else break;
    
    /* An error was generated, Unless it was a system error stop */
  case -1:
    if (errno == EINTR)
      goto receive;
    else goto handle_error;
  }
  /* Did something arrive for this socket */
  if ((read_descriptors & mask) == 0)
    goto receive;
  
  /* Something arrived; try to get it */
  
 getbuf:
  fromlen = sizeof(struct sockaddr);
  received = recvfrom(s, inbuf, UDP_DATA_BLOCK_SIZE, 0, 
                      (struct sockaddr *)&from, &fromlen);
  if (received < 0)
    switch(errno) {
    case EINTR:
      goto getbuf;
    case EWOULDBLOCK:
      goto receive;
    default:
      goto handle_error;
    }
  
  /* close the socket */
  close(s);
  
  /* Return TRUE */
  return(ATOM_T);
  
  /* Return NIL; Eventually we will need to return something more informative, perhaps errno 
   * would be sufficient.
   */
 handle_error:
  return(NIL_PTR);
#endif /* DOS */

}

