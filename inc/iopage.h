/* @(#) iopage.h Version 2.7 (4/21/92). copyright Venue & Fuji Xerox  */


/************************************************************************/
/*									*/
/*	Copyright 1989, 1990 Venue, Fuji Xerox Co., Ltd, Xerox Corp.	*/
/*									*/
/*	This file is work-product resulting from the Xerox/Venue	*/
/*	Agreement dated 18-August-1989 for support of Medley.		*/
/*									*/
/************************************************************************/


/*
 *	Copyright (C) 1987 by Fuji Xerox Co., Ltd. All rights reserved.
 *
 *	Author	:	Osamu Nakamura
 */
/****************************************************************
 *								*
 *	iopage.h		July 6, 1987 by Osamu Nakamura	*
 *								*
 ****************************************************************/

#ifndef IOPAGE_DEFINED
#define IOPAGE_DEFINED	(1)
#ifndef BYTESWAP
/* Normal definition, for big-indian machines */
typedef struct iopage {
	DLword	 dummy0[022];
	DLword   dlmaintpanel;
        DLword   dlfloppycmd;
        DLword   dlttyportcmd;
        DLword   dlprocessorcmd;
        DLword   newmousestate;
        DLword   dlbeepcmd;
        DLword   dlrs232cmisccommand;
        DLword   dlrs232cputflag;
	DLword   dlrs232cgetflag;
	DLword   dummy1[6];
	DLword   dlfloppy;
	DLword   dlttyout;
	DLword   dummy2;
	DLword   dlttyin;
	DLword   dummy3;
	DLword   dlprocessor2;
	DLword   dlprocessor1;
	DLword   dlprocessor0;
	DLword   newmousex;
	DLword   newmousey;
	DLword   dlbeepfreq;
	DLword   dlrs232cparametercsblo;
	DLword   dlrs232cparametercsbhi;
	DLword   dlrs232csetrs366status[3];
	DLword   dlrs232cputcsblo;
	DLword   dlrs232cputcsbhi;
	DLword   dlrs232cgetcsblo;
	DLword   dlrs232cgetcsbhi;
	DLword   dlrs232cdevicestatus;
	DLword   dlrs232cparameteroutcome;
	DLword   dltodvalid;
	DLword   dltodlo;
	DLword   dltodhi;
	DLword   dltodlo2;
	DLword   dlmousex;
	DLword   dlmousey;
	DLword   dlutilin;
	DLword   dlkbdad0;
	DLword   dlkbdad1;
	DLword   dlkbdad2;
	DLword   dlkbdad3;
	DLword   dlkbdad4;
	DLword   dlkbdad5;
	DLword   dllsepimagecsb[040];
	DLword   dliophardwareconfig;
	DLword   dummy4[013];
	DLword   dlrs232cparametercsblo_11;
	DLword   dlrs232cparametercsbhi_11;
	DLword   dlrs232csetrs366status_11[016] ;
	DLword   dummy5[074];
	DLword   dlmagtape[4];
	DLword   dlethernet[014];
	DLword   dummy6[037];
	DLword   dldispinterrupt;
	DLword   dldispcontrol;
	DLword   dldispborder;
	DLword   dlcursorx;
	DLword   dlcursory;
	DLword   dlcursorbitmap[020];
} IOPAGE;
#else
	/***********************************************************/
	/*       Byte-swapped/word-swapped version, for 386i       */
	/***********************************************************/
typedef struct iopage {
	DLword	 dummy0[022];
        DLword   dlfloppycmd;
	DLword   dlmaintpanel;	/* hi */
        DLword   dlprocessorcmd;
        DLword   dlttyportcmd;	/* hi */
        DLword   dlbeepcmd;
        DLword   newmousestate;	/* hi */
        DLword   dlrs232cputflag;
        DLword   dlrs232cmisccommand;	/* hi */
	DLword	 dummy1b;
	DLword   dlrs232cgetflag;	/* hi */
	DLword   dummy1[4];
	DLword   dlfloppy;
	DLword	 dummy1a;	/* hi */
	DLword   dummy2;
	DLword   dlttyout;	/* hi */
	DLword   dummy3;
	DLword   dlttyin;	/* hi */
	DLword   dlprocessor1;
	DLword   dlprocessor2;	/* hi */
	DLword   newmousex;
	DLword   dlprocessor0;	/* hi */
	DLword   dlbeepfreq;
	DLword   newmousey;	/* hi */
	DLword   dlrs232cparametercsbhi;
	DLword   dlrs232cparametercsblo;	/* hi */
	DLword   dlrs232csetrs366status[2];
	DLword   dlrs232cputcsblo;
	DLword	 dlrs232csetrs366statusa; /* hi */
	DLword   dlrs232cgetcsblo;
	DLword   dlrs232cputcsbhi;	/* hi */
	DLword   dlrs232cdevicestatus;
	DLword   dlrs232cgetcsbhi;	/* hi */
	DLword   dltodvalid;
	DLword   dlrs232cparameteroutcome;	/* hi */
	DLword   dltodhi;
	DLword   dltodlo;	/* hi */
	DLword   dlmousex;
	DLword   dltodlo2;	/* hi */
	DLword   dlutilin;
	DLword   dlmousey;	/* hi */
	DLword   dlkbdad1;
	DLword   dlkbdad0;	/* hi */
	DLword   dlkbdad3;
	DLword   dlkbdad2;	/* hi */
	DLword   dlkbdad5;
	DLword   dlkbdad4;	/* hi */
	DLword   dllsepimagecsb[040];
	DLword	 dummy4a;
	DLword   dliophardwareconfig;	/* hi */
	DLword   dummy4[012];
	DLword   dlrs232cparametercsbhi_11;
	DLword   dlrs232cparametercsblo_11;	/* hi */
	DLword   dlrs232csetrs366status_11[016] ;
	DLword   dummy5[074];
	DLword   dlmagtape[4];
	DLword   dlethernet[014];
	DLword   dummy6[036];
	DLword   dldispinterrupt;
	DLword	 dummy6a;
	DLword   dldispborder;
	DLword   dldispcontrol;	/* hi */
	DLword   dlcursory;
	DLword   dlcursorx;	/* hi */
	DLword   dlcursorbitmap[020];
} IOPAGE;
#endif /* BYTESWAP */

#endif
