/* @(#) XClose.c Version 1.5 (4/27/92). copyright venue & Fuji Xerox  */
static char *id = "@(#) XClose.c	1.5 4/27/92	(venue & Fuji Xerox)";




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




#include <stdio.h>

#ifdef ISC
#include <sys/bsdtypes.h>
#include <stropts.h>
#include <sys/ioctl.h>
#endif ISC

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "XVersion.h"
#include "dbprint.h"
#include "dspif.h"

