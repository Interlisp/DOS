/* @(#) ifpage.h Version 2.9 (4/21/92). copyright Venue & Fuji Xerox  */

/*
 *	Copyright (C) 1987 by Fuji Xerox Co., Ltd. All rights reserved.
 *
 *	Author	:	Osamu Nakamura
 */
/****************************************************************
 *								*
 *	ifpage.h		July 6, 1987 by Osamu Nakamura	*
 *				12/9/88 JDS for byte swapped    *
 *					    (reverse bytes & wr)*
 *								*
 ****************************************************************/


/************************************************************************/
/*									*/
/*	Copyright 1989, 1990 Venue, Fuji Xerox Co., Ltd, Xerox Corp.	*/
/*									*/
/*	This file is work-product resulting from the Xerox/Venue	*/
/*	Agreement dated 18-August-1989 for support of Medley.		*/
/*									*/
/************************************************************************/


#ifndef IFPAGE_DEFINED
#define IFPAGE_DEFINED	(1)
#define IFPAGE_KEYVAL 0x15e3
#ifndef BYTESWAP
/* Normal definition, for big-indian machines */
typedef struct ifpage {
	DLword    currentfxp;
	DLword    resetfxp;
	DLword    subovfxp;
	DLword    kbdfxp;
	DLword    hardreturnfxp;
	DLword    gcfxp;
	DLword    faultfxp;
	DLword    endofstack;
	DLword    lversion;
	DLword    minrversion;
	DLword    minbversion;
	DLword    rversion;
	DLword    bversion;
	DLword    machinetype;
	DLword    miscfxp;
	DLword    key;
	DLword    serialnumber;
	DLword    emulatorspace;
	DLword    screenwidth;
	DLword    nxtpmaddr;
	DLword    nactivepages;
	DLword    ndirtypages;
	DLword    filepnpmp0;
	DLword    filepnpmt0;
	DLword    teleraidfxp;
	DLword    filler1;
	DLword    filler2;
	DLword    filler3;
	DLword    usernameaddr;
	DLword    userpswdaddr;
	DLword    stackbase;
	DLword    faulthi;
	DLword    faultlo;
	DLword    devconfig;/*wasrealpagetable;*/
	DLword    rptsize;
	DLword    rpoffset;
	DLword    wasrptlast;
	DLword    embufvp;
	DLword    nshost0;
	DLword    nshost1;
	DLword    nshost2;
	DLword    mdszone;
	DLword    mdszonelength;
	DLword    emubuffers;
	DLword    emubuflength;
#ifdef NEW_STORAGE
	DLword    process_size; /* was lastnumchars */
	DLword    storagefullstate;  /* was sysdisk */
#else 
	DLword    lastnumchars; 
	DLword    sysdisk;  
#endif /* NEW_STORAGE */

	DLword    isfmap;
	/* these are for \miscapply
	 * -- note that they are not ref counted, so don't pass the only pointer
	 * to something this way */
	LispPTR   miscstackfn;
	LispPTR   miscstackarg1;
	LispPTR   miscstackarg2;
	LispPTR   miscstackresult;
	DLword    nrealpages;
	DLword    lastlockedfilepage;
	DLword    lastdominofilepage;
	DLword    fptovpstart;
	DLword    fakemousebits;
	DLword    dl24bitaddressable;
	LispPTR   realpagetableptr;
	DLword    dllastvmempage;
	DLword    fullspaceused;
	DLword    fakekbdad4;
	DLword    fakekbdad5;
} IFPAGE;
#else
	/***********************************************************/
	/*       Byte-swapped/word-swapped version, for 386i       */
	/***********************************************************/
typedef struct ifpage {
	DLword    resetfxp;
	DLword    currentfxp;  /* hi word */
	DLword    kbdfxp;
	DLword    subovfxp;  /* hi word */
	DLword    gcfxp;
	DLword    hardreturnfxp;  /* hi word */
	DLword    endofstack;
	DLword    faultfxp;  /* hi word */
	DLword    minrversion;
	DLword    lversion;  /* hi word */
	DLword    rversion;
	DLword    minbversion;  /* hi word */
	DLword    machinetype;
	DLword    bversion;  /* hi word */
	DLword    key;
	DLword    miscfxp;  /* hi word */
	DLword    emulatorspace;
	DLword    serialnumber;  /* hi word */
	DLword    nxtpmaddr;
	DLword    screenwidth;  /* hi word */
	DLword    ndirtypages;
	DLword    nactivepages;  /* hi word */
	DLword    filepnpmt0;
	DLword    filepnpmp0;  /* hi word */
	DLword    filler1;
	DLword    teleraidfxp;  /* hi word */
	DLword    filler3;
	DLword    filler2;  /* hi word */
	DLword    userpswdaddr;
	DLword    usernameaddr;  /* hi word */
	DLword    faulthi;
	DLword    stackbase;  /* hi word */
	DLword    devconfig;/*wasrealpagetable;*/
	DLword    faultlo;  /* hi word */
	DLword    rpoffset;
	DLword    rptsize;  /* hi word */
	DLword    embufvp;
	DLword    wasrptlast;  /* hi word */
	DLword    nshost1;
	DLword    nshost0;  /* hi word */
	DLword    mdszone;
	DLword    nshost2;  /* hi word */
	DLword    emubuffers;
	DLword    mdszonelength;  /* hi word */
#ifdef NEW_STORAGE
	DLword    process_size;
	DLword    emubuflength;  /* hi word */
	DLword    isfmap;
	DLword    storagefullstate;  /* hi word */
#else
	DLword    lastnumchars;
	DLword    emubuflength;  /* hi word */
	DLword    isfmap;
	DLword    sys_disk;  /* hi word */
#endif /* NEW_STORAGE */

	/* these are for \miscapply
	 * -- note that they are not ref counted, so don't pass the only pointer
	 * to something this way */
	LispPTR   miscstackfn;
	LispPTR   miscstackarg1;
	LispPTR   miscstackarg2;
	LispPTR   miscstackresult;
	DLword    lastlockedfilepage;
	DLword    nrealpages; /* hi word */
	DLword    fptovpstart;
	DLword    lastdominofilepage; /* hi word */
	DLword    dl24bitaddressable;
	DLword    fakemousebits; /* hi word */
	LispPTR   realpagetableptr;
	DLword    fullspaceused;
	DLword    dllastvmempage; /* hi word */
	DLword    fakekbdad5;
	DLword    fakekbdad4; /* hi word */
} IFPAGE;
#endif /* BYTESWAP */

#endif
