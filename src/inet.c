/* @(#) inet.c Version 1.19 (4/21/92). copyright venue & Fuji Xerox  */
static char *id = "@(#) inet.c	1.19 4/21/92	(venue & Fuji Xerox)";


/************************************************************************/
/*									*/
/*	Copyright 1989, 1990, 1991 Venue				*/
/*									*/
/*	This file is NOT work-product resulting from the Xerox/Venue	*/
/*	Agreement dated 18-August-1989 for support of Medley.		*/
/*									*/
/************************************************************************/



#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#ifndef DOS
#include <sys/file.h> 
#include <signal.h>
#ifdef ISC
#include <sys/fcntl.h>
#include <sys/bsdtypes.h>
#endif /* ISC */
 
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
 
#include <sys/ioctl.h>
#include <errno.h>
#include "lispemul.h"
#include "lispmap.h"
#include "lsptypes.h"
#include "arith.h"
#include "emlglob.h"
#include "lspglob.h"
#include "adr68k.h"
#include "ether.h"
#include "dbprint.h"
#include "locfile.h"

#ifdef HPUX
#define FASYNC O_NONBLOCK
#endif /* NPUX */

#ifdef ISC
#define FASYNC O_NONBLOCK
#define SIGIO SIGPOLL
#endif /* ISC */


#define TCPhostlookup 0
#define TCPservicelookup 1
#define TCPsocket	2
#define TCPclose	3
#define	TCPconnect	4
#define TCPsend		5
#define TCPrecv		6
#define TCPListen	7
#define TCPAccept	8

#define INETpeername	64	/* (socket buf) => buf has name, returns len */
#define INETpeeraddr	65
#define INETgetname	66	/* Address to name translation */

#define UDPListen	128
#define UDPConnect	129
#define UDPSendto	130
#define UDPRecvfrom	131


extern u_int LispIOFds, LispReadFds;
extern int *Lisp_errno;

subr_TCP_ops (op, nameConn, proto, length, bufaddr, maxlen)
  int op;
  LispPTR nameConn, proto, length, bufaddr, maxlen; 
  {
    int sock, len, buflen, res;
    char namestring[100];
    char servstring[50];
    struct sockaddr_in addr;
    struct hostent *host;
    struct servent *service;
    struct sockaddr_in farend;
    int addr_class, protocol;
    char *buffer;
    int result;
#ifdef RS6000
    static int one=1;		/* Used in TCPconnect */
#endif

    switch (op & 0xFFFF)
      {
	case TCPhostlookup:	LispStringToCString(nameConn, namestring, 100);
				host = gethostbyname(namestring);
				if (!host) return(NIL);
				N_ARITH_SWITCH(*(int *)host->h_addr);
				break;

	case TCPservicelookup:	LispStringToCString(nameConn, namestring, 100);
				LispStringToCString(proto, servstring, 50);
				service = getservbyname(namestring, servstring);
				if (!service) return(NIL);
				return(GetSmallp(service->s_port));
				break;

	case TCPsocket:		addr_class = LispNumToCInt(nameConn);
				protocol = LispNumToCInt(proto);
				result = socket(addr_class, protocol, 0);
#ifdef RS6000
				ioctl(result, FIONBIO, &one);
				fcntl(result, F_SETOWN, getpid());
#else
#ifdef HPUX
				ioctl(result, FIOSNBIO, &one);
#else
				fcntl(result, F_SETFL,  fcntl(result, F_GETFL, 0)
					      | FNDELAY | FASYNC);
#ifndef ISC
				fcntl(result, F_SETOWN, getpid());
#endif /* ISC */

#endif /* HPUX */

#endif /* RS6000 */

				return(GetSmallp(result));
				break;

	case TCPconnect:	/* args: hostname, socket# */
	  			bzero(&farend, sizeof farend);
				N_GETNUMBER(nameConn, res, string_host);
				farend.sin_addr.s_addr = htons(res);
				goto host_ok;
			      string_host:
				LispStringToCString(nameConn, namestring, 100);
				host = gethostbyname(namestring);
				if (!host) return(NIL);
				bcopy( (char *)host->h_addr,
				       (char *)&farend.sin_addr,
				       host->h_length);
			      host_ok:
				sock = LispNumToCInt(proto);
				result = socket(AF_INET, SOCK_STREAM, 0);
				farend.sin_family = AF_INET;
				bcopy( (char *)host->h_addr,
				       (char *)&farend.sin_addr,
				       host->h_length);
				farend.sin_port = sock;
				if (connect(result, &farend, sizeof farend)<0)
				  {
				    perror("TCP connect");
				    return(NIL);
				  }
#ifdef RS6000
				/* FNDELAY alone isn't enough on aix */
				/* (don't know if FIONBIO alone is enough) */
				ioctl(result, FIONBIO, &one);
				fcntl(result, F_SETOWN, getpid());
#else
#ifdef HPUX
				ioctl(result, FIOSNBIO, &one);
#else
				fcntl(result, F_SETFL,  fcntl(result, F_GETFL, 0)
					      | FNDELAY);
#ifndef ISC
				fcntl(result, F_SETOWN, getpid());
#endif /* ISC */

#endif /* HPUX */

#endif /* RS6000 */

				return(GetSmallp(result));
				break;

	case TCPsend:		/* args: conn, buffer, len */
				sock = LispNumToCInt(nameConn);
				buffer = (char *)Addr68k_from_LADDR(proto);
				len = LispNumToCInt(length);
				DBPRINT(("sock: %d, len %d.\n", sock, len));

#ifdef BYTESWAP
				word_swap_page(buffer, (len + 3) >> 2);
#endif /* BYTESWAP */


				result = send(sock, buffer, len, 0);

#ifdef BYTESWAP
				word_swap_page(buffer, (len + 3) >> 2);
#endif /* BYTESWAP */


				if (result < 0)
				  {
				    perror("TCP send"); return(NIL);
				  }
				return(GetSmallp(result));
				break;

	case TCPrecv:		/* args: conn, buffer, maxlen */
				sock = LispNumToCInt(nameConn);
				buffer = (char *)Addr68k_from_LADDR(proto);
				len = LispNumToCInt(length);
				result = read(sock, buffer, len);
				if (result < 0)
				  {
				    if (errno == EWOULDBLOCK)
				      return(ATOM_T);
				    perror("TCP read");
				    *Lisp_errno = errno;
				    return(NIL);
				  }
#ifdef BYTESWAP
				word_swap_page(buffer, (result + 3) >> 2);
#endif /* BYTESWAP */

				return(GetSmallp(result));
				break;


	case TCPclose:		sock = LispNumToCInt(nameConn);
				LispIOFds &= ~(1<<sock);
				LispReadFds &= ~(1<<sock);
				shutdown(sock, 2);
				close(sock);
				return(ATOM_T);

	case TCPListen:		/* socket# to listen on */
				sock = LispNumToCInt(nameConn);
				result = socket(AF_INET, SOCK_STREAM, 0);
				farend.sin_family = AF_INET;
				farend.sin_port = sock;
				farend.sin_addr.s_addr = INADDR_ANY;
				if (bind(result, &farend, sizeof(farend))<0)
				  {
				    perror("TCP bind");
				    close(result);
				    return(NIL);
				  }
				{ /* Do this without taking IO interrupts */
#ifdef SYSVSIGNALS
				sighold(SIGIO);
#else
				  int oldmask = sigblock(sigmask(SIGIO));
#endif /* SYSVSIGNALS */

#ifdef RS6000
				ioctl(result, FIONBIO, &one);
				fcntl(result, F_SETOWN, getpid());
#else
#ifdef HPUX
				ioctl(result, FIOSNBIO, &one);
#else
				fcntl(result, F_SETFL,
				        fcntl(result, F_GETFL, 0) | FNDELAY
								  | FASYNC);
#ifndef ISC
				fcntl(result, F_SETOWN, getpid());
#endif /* ISC */

#endif /* HPUX */

#endif /* RS6000 */

				
				  if (listen(result, 5) == -1)
				    {
				      perror("TCP Listen");
				      close(result);
#ifdef SYSVSIGNALS
				      sigrelse(SIGIO);
#else
				      sigsetmask(oldmask);
#endif /* SYSVSIGNALS */

				      return(NIL);
				    }
#ifdef SYSVSIGNALS
				sigrelse(SIGIO);
#else
				  sigsetmask(oldmask);
#endif /* SYSVSIGNALS */

				}
				LispIOFds |= (1 << result); /* so we get interrupts */
				LispReadFds |= LispIOFds;
				DBPRINT(("LispIOFds = 0x%x.\n", LispIOFds));
				return(GetSmallp(result));
				break;

	case TCPAccept:		/* Socket we're listening on */
				sock = LispNumToCInt(nameConn);
				result = accept(sock, NULL, 0);
				if (result < 0)
				  {
				    if (errno != EWOULDBLOCK)
				      perror("TCP Accept");
				    return(NIL);
				  }
#ifdef RS6000
				ioctl(result, FIONBIO, &one);
				fcntl(result, F_SETOWN, getpid());
#else
#ifdef HPUX
				ioctl(result, FIOSNBIO, &one);
#else
				fcntl(result, F_SETFL,  fcntl(result, F_GETFL, 0)
					      | FNDELAY);
#ifndef ISC
				fcntl(result, F_SETOWN, getpid());
#endif /* ISC */

#endif /* HPUX */

#endif /* RS6000 */

				return(GetSmallp(result));
				break;

	case INETpeername:	/* socket#, buffer for name string */
				sock = LispNumToCInt(nameConn);
				buffer = (char *)Addr68k_from_LADDR(proto);
				res = sizeof(addr);
				getpeername(sock, &addr, &res);
				host = gethostbyaddr(&addr, res, AF_INET);
				strcpy(buffer, host->h_name);
				return(GetSmallp(strlen(host->h_name)));
				break;

	case INETgetname:	/* host addr, buffer for name string */
				sock = LispNumToCInt(nameConn);
				buffer = (char *)Addr68k_from_LADDR(proto);
				res = sizeof(addr);
				addr.sin_addr.s_addr = sock;
				host = gethostbyaddr(&addr, res, 0);
				if (!host) return(GetSmallp(0));
				strcpy(buffer, host->h_name);
				return(GetSmallp(strlen(host->h_name)));
				break;

	case UDPListen:		/* socket# to listen on */
				sock = LispNumToCInt(nameConn);
				result = socket(AF_INET, SOCK_DGRAM, 0);
				farend.sin_family = AF_INET;
				farend.sin_port = sock;
				farend.sin_addr.s_addr = INADDR_ANY;
				if (bind(result, &farend, sizeof(farend))<0)
				  {
				    perror("UDP bind");
				    close(result);
				    return(NIL);
				  }
#ifdef RS6000
				ioctl(result, FIONBIO, &one);
				fcntl(result, F_SETOWN, getpid());
#else
#ifdef HPUX
				ioctl(result, FIOSNBIO, &one);
#else
				fcntl(result, F_SETFL,  fcntl(result, F_GETFL, 0)
					      | FNDELAY | FASYNC);
#ifndef ISC
				fcntl(result, F_SETOWN, getpid());
#endif /* ISC */

#endif /* HPUX */

#endif /* RS6000 */

				LispIOFds |= (1 << result); /* so we get interrupts */
				LispReadFds |= LispIOFds;
				DBPRINT(("LispIOFds = 0x%x.\n", LispIOFds));
				return(GetSmallp(result));
				break;

	case UDPSendto:		/* fd-socket# addr remote-socket buffer len*/
				sock = LispNumToCInt(nameConn);
				farend.sin_family = AF_INET;
				farend.sin_port = LispNumToCInt(length);
				farend.sin_addr.s_addr = LispNumToCInt(proto);
				buffer = (char *) Addr68k_from_LADDR(bufaddr);
				buflen = LispNumToCInt(maxlen);

				DBPRINT(("UDP send:  socket = %d, remote-port = %d.\n",
					  sock, farend.sin_port));
				DBPRINT(("           remote-addr = 0x%x, buflen = %d.\n",
					  farend.sin_addr.s_addr, buflen));


#ifdef BYTESWAP
				word_swap_page(buffer, (buflen + 3) >> 2);
#endif /* BYTESWAP */


				if (result = sendto(sock, buffer, buflen, 0,
						    &farend, sizeof farend)  <0)

#ifdef BYTESWAP
				word_swap_page(buffer, (buflen + 3) >> 2);
#endif /* BYTESWAP */


				  {
				    perror("UDP Send");
				    printf(" fd = %d, addr = 0x%x.\n", sock,
						farend.sin_addr.s_addr);
				    return(NIL);
				  }
				return(GetSmallp(result));
				break;

	case UDPRecvfrom:	/* fd-socket# buffer len addr-cell port-cell*/
				sock = LispNumToCInt(nameConn);
				buffer = (char *)Addr68k_from_LADDR(proto);
				buflen = LispNumToCInt(length);
				res = sizeof farend;
				if ((result = recvfrom(sock, buffer, buflen, 0,
						    &farend, &res))  <0)
				  {
				    perror("UDP Recv");
				    return(NIL);
				  }

				DBPRINT(("UDP recv:  socket = %d, len = %d.\n",
					 sock, result));
				DBPRINT(("           remote-addr = 0x%x, remote-port = %d.\n",
					 farend.sin_addr.s_addr, farend.sin_port));
				DBPRINT(("           bufsize = %d, addrcell = 0x%x, portcell = 0x%x.\n",
					 buflen, bufaddr, maxlen));

				*((int *)Addr68k_from_LADDR(bufaddr)) = (int)farend.sin_addr.s_addr;
				*((int *)Addr68k_from_LADDR(maxlen))  = (int)farend.sin_port;

#ifdef BYTESWAP
				word_swap_page(buffer, (result + 3) >> 2);
#endif /* BYTESWAP */

				return(GetSmallp(result));
				break;


	default: return(NIL);
		 break;
      }

  }
#endif /* DOS */
