/* @(#) Xcolor.c Version 1.3 (5/27/92). copyright venue & Fuji Xerox  */
static char *id = "@(#) Xcolor.c	1.3 5/27/92	(venue & Fuji Xerox)";
/*
*
*
* Copyright (C) 1988 by Fuji Xerox co.,Ltd. All rights reserved.
*
*		Author: Mitsunori Matsuda
*		Date  : August 18, 1988
*/

#include <stdio.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "XVersion.h"

#define FALSE 0
#define TRUE  !FALSE

#include "dspif.h"
extern DspInterface currentdsp;

extern XGCValues gcv;
extern GC cursor_source_gc;

