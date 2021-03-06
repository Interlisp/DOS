/* This is G-file @(#) array.h Version 2.2 (3/16/89). copyright Venue & Fuji Xerox  */
/****************************************************************************/
/****************************************************************************/
/*	Copyright (C) 1987 by Fuji Xerox Co., Ltd. All rights reserved.	    */
/****************************************************************************/
/*                                                                          */
/*                    File name : array.h                                   */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/*                    Creation Date : Oct-5,1987                            */
/*                    Written by Tomoru Teruuchi                            */
/*                            Modified : April-22,1988                      */
/****************************************************************************/
/*                                                                          */
/*         Herein, Entries : sequencedescriptor                             */
/*                           arrayp                                         */
/*                           arrayblock                                     */
/*                           arrayheader (added structure)                  */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
/*                                                                    Tomtom*/
/****************************************************************************/


/************************************************************************/
/*									*/
/*	Copyright 1989, 1990 Venue, Fuji Xerox Co., Ltd, Xerox Corp.	*/
/*									*/
/*	This file is work-product resulting from the Xerox/Venue	*/
/*	Agreement dated 18-August-1989 for support of Medley.		*/
/*									*/
/************************************************************************/


#ifndef BYTESWAP
	/********************************/
	/*  Normal byte-order version   */
	/********************************/
typedef struct sequencedescriptor {
	unsigned	orig		:1;
	unsigned	nil		:1;
	unsigned	readonly	:1;
	unsigned 	nil2		:1;
	unsigned	typ		:4;
	unsigned	base		:24;
	DLword		length;
	DLword		offst;
} Arrayp;

struct arrayheader {
        unsigned        nil             :8;
	unsigned        base            :24;
	unsigned        readonlyp       :1;
	unsigned        indirectp       :1;
	unsigned        bitp            :1;
	unsigned        stringp         :1;
	unsigned        adjustablep     :1;
	unsigned        displacedp      :1;
	unsigned        fillpointerp    :1;
	unsigned        extendablep     :1;
	unsigned        typenumber      :8;
	DLword          offset;
	DLword          fillpointer;
	DLword          totalsize;
      };

struct arrayblock {
	unsigned	password	:13;
	unsigned	gctype		:2;
	unsigned	inuse		:1;
	DLword		arlen;
	LispPTR		fwd;
	LispPTR		bkwd;
};

struct abdum {
	DLword		abflags;
};


#else
	/********************************/
	/*  Byte-swapped version, for   */
	/*  e.g., 80386's		*/
	/********************************/
typedef struct sequencedescriptor {
	unsigned	base		:24;
	unsigned	typ		:4;
	unsigned 	nil2		:1;
	unsigned	readonly	:1;
	unsigned	nil		:1;
	unsigned	orig		:1;
	DLword		offst;
	DLword		length;
} Arrayp;

struct arrayheader {
	unsigned        base            :24;
        unsigned        nil             :8;
	DLword          offset;
	unsigned        typenumber      :8;
	unsigned        extendablep     :1;
	unsigned        fillpointerp    :1;
	unsigned        displacedp      :1;
	unsigned        adjustablep     :1;
	unsigned        stringp         :1;
	unsigned        bitp            :1;
	unsigned        indirectp       :1;
	unsigned        readonlyp       :1;
	DLword          totalsize;
	DLword          fillpointer;
      };

struct arrayblock {
	DLword		arlen;
	unsigned	inuse		:1;
	unsigned	gctype		:2;
	unsigned	password	:13;
	LispPTR		fwd;
	LispPTR		bkwd;
};

struct abdum
  {
	DLword		abflags;
};


#endif /* BYTESWAP */




/****************************************************************************/
/*                                                                          */
/*        The following data are the constant values for array itself.      */
/*                                                                          */
/****************************************************************************/

/* #define ST_BYTE			0 in lsptypes.h */
/* #define ST_POS16			1 in lsptypes.h */
#define ST_INT32		2
/*#define ST_CODE			4 in load.h      */
#define ST_PTR			6
#define ST_FLOAT		7
#define ST_BIT			8
#define ST_PTR2			11

/****************************************************************************/
/*                                                                          */
/*        The following data are the constant values for array reclaimer.   */
/*                                                                          */
/****************************************************************************/

#define MAXARRAYBLOCKSIZE	65535
#define MAXARRAYLEN		65535
#define ARRAYBLOCKHEADERCELLS	1
#define ARRAYBLOCKTRAILERCELLS	1
#define ARRAYBLOCKOVERHEADCELLS	(ARRAYBLOCKHEADERCELLS+ARRAYBLOCKTRAILERCELLS)
#define MAXARRAYNCELLS		(MAXARRAYBLOCKSIZE-ARRAYBLOCKOVERHEADCELLS)
#define ARRAYBLOCKHEADERWORDS	2
#define ARRAYBLOCKTRAILERWORDS	2
#define ARRAYBLOCKOVERHEADWORDS	(ARRAYBLOCKHEADERWORDS+ARRAYBLOCKTRAILERWORDS)
#define ARRAYBLOCKLINKINGCELLS	2
#define MINARRAYBLOCKSIZE	(ARRAYBLOCKOVERHEADCELLS+ARRAYBLOCKLINKINGCELLS)
#define MAXBUCKETINDEX		30
#define UNBOXEDBLOCK_GCT	0
#define PTRBLOCK_GCT		1
#define CODEBLOCK_GCT		2
#define ABPASSWORDSHIFT		3
#define ARRAYBLOCKPASSWORD	(43690 >> ABPASSWORDSHIFT)
				/* = 1010101010101010 >> 3 = 5461 */
#define FREEARRAYFLAGWORD	((ARRAYBLOCKPASSWORD << ABPASSWORDSHIFT) | (UNBOXEDBLOCK_GCT << 1))
				/* = 43688 */
#define USEDARRAYFLAGWORD	((ARRAYBLOCKPASSWORD << ABPASSWORDSHIFT) | 1)
#define CODEARRAYFLAGWORD	((ARRAYBLOCKPASSWORD << ABPASSWORDSHIFT) | ((CODEBLOCK_GCT << 1) | 1))
#define FIRSTARRAYSEGMENT	19
#define MAXCELLSPERHUNK		64

/****************** The following are for codereclaimer *********************/

#define BITSPERBITE		8

/********************* End of codereclaimer *********************************/



/****************************************************************************/
/*                                                                          */
/*                  End of Definitions                                      */
/*                                                                          */
/****************************************************************************/
