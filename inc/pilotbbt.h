/* @(#) pilotbbt.h Version 2.5 (4/21/92). copyright envos & Fuji Xerox  */

/*
 *	Copyright (C) 1987 by Fuji Xerox Co., Ltd. All rights reserved.
 *
 *	Author	:	Osamu Nakamura
 */


/************************************************************************/
/*									*/
/*	Copyright 1989, 1990 Venue, Fuji Xerox Co., Ltd, Xerox Corp.	*/
/*									*/
/*	This file is work-product resulting from the Xerox/Venue	*/
/*	Agreement dated 18-August-1989 for support of Medley.		*/
/*									*/
/************************************************************************/




/****************************************************************
 *								*
 *	pilotbbt.h		July 6, 1987 by Osamu Nakamura	*
 *								*
 ****************************************************************/

#ifndef BYTESWAP
	/**********************************/
	/* Normal byte-order declarations */
	/**********************************/
typedef struct pilotbbt{
	DLword	pbtdestlo;
        DLword  pbtdesthi;
        DLword  pbtdestbit;	/* destination bit address */
        short   pbtdestbpl;     /* destination bits per line
				 *  -- distance in bits to move between items */
        DLword  pbtsourcelo;
        DLword  pbtsourcehi;
        DLword  pbtsourcebit;	/* source bit address */
        short   pbtsourcebpl;	/* source bits per line */
        DLword  pbtwidth;	/* width of an item in bits */
        DLword  pbtheight;	/* number of items -- height in scanlines */
        unsigned	pbtbackward : 1;
        unsigned        pbtdisjoint : 1;
        unsigned        pbtdisjointitems : 1;
        unsigned        pbtusegray  : 1;
        unsigned        pbtsourcetype : 1;
        unsigned        pbtoperation : 2;
        unsigned        nil  : 9;
        DLword  dummy[5];	/* unused, needed to make 16-alignment */
} PILOTBBT;


typedef struct texturebbt{
	DLword	pbtdestlo;
        DLword  pbtdesthi;
        DLword  pbtdestbit;     /* destination bit address */
        short   pbtdestbpl;     /* destination bits per line
				 * -- distance in bits to move between items */
        DLword  pbtsourcelo;
        DLword  pbtsourcehi;
        DLword  pbtsourcebit;	/* source bit address */
        unsigned	dummy2 : 4;
				/* overlay on pbtsourcebpl when pbtusegray */
        unsigned	pbtgrayoffset : 4;
				/* offset in gray block where bitblt should start */
        unsigned	pbtgraywidthlessone : 4;
				/* width-1 of gray block in words */
        unsigned	pbtgrayheightlessone : 4;
				/* height-1 of gray block */
        DLword  pbtwidth;	/* width of an item in bits */
        DLword  pbtheight;	/* number of items -- height in scanlines */
        unsigned	pbtbackward : 1;
        unsigned        pbtdisjoint : 1;
        unsigned        pbtdisjointitems : 1;
        unsigned        pbtusegray  : 1;
        unsigned        pbtsourcetype : 1;
        unsigned        pbtoperation : 2;
        unsigned        nil  : 9;
        DLword  dummy[5];	/* unused, needed to make 16-alignment */
} TEXTUREBBT;

#else
	/****************************/
	/* Byte-swapped definitions */
	/****************************/

typedef struct pilotbbt
  {
        DLword  pbtdesthi;
	DLword	pbtdestlo;
        short   pbtdestbpl;     /* destination bits per line
				 *  -- distance in bits to move between items */
        DLword  pbtdestbit;	/* destination bit address */
        DLword  pbtsourcehi;
        DLword  pbtsourcelo;
        short   pbtsourcebpl;	/* source bits per line */
        DLword  pbtsourcebit;	/* source bit address */
        DLword  pbtheight;	/* number of items -- height in scanlines */
        DLword  pbtwidth;	/* width of an item in bits */
	DLword	dummy1;
        unsigned        nil  : 9;
        unsigned        pbtoperation : 2;
        unsigned        pbtsourcetype : 1;
        unsigned        pbtusegray  : 1;
        unsigned        pbtdisjointitems : 1;
        unsigned        pbtdisjoint : 1;
        unsigned	pbtbackward : 1;
        DLword  dummy[4];	/* unused, needed to make 16-alignment */
  } PILOTBBT;


typedef struct texturebbt
  {
        DLword  pbtdesthi;
	DLword	pbtdestlo;
        short   pbtdestbpl;     /* destination bits per line
				 * -- distance in bits to move between items */
        DLword  pbtdestbit;     /* destination bit address */
        DLword  pbtsourcehi;
        DLword  pbtsourcelo;
        unsigned	pbtgrayheightlessone : 4;
				/* height-1 of gray block */
        unsigned	pbtgraywidthlessone : 4;
				/* width-1 of gray block in words */
        unsigned	pbtgrayoffset : 4;
				/* offset in gray block where bitblt should start */
        unsigned	dummy2 : 4;
				/* overlay on pbtsourcebpl when pbtusegray */
        DLword  pbtsourcebit;	/* source bit address */
        DLword  pbtheight;	/* number of items -- height in scanlines */
        DLword  pbtwidth;	/* width of an item in bits */
	DLword	dummy1;
        unsigned        nil  : 9;
        unsigned        pbtoperation : 2;
        unsigned        pbtsourcetype : 1;
        unsigned        pbtusegray  : 1;
        unsigned        pbtdisjointitems : 1;
        unsigned        pbtdisjoint : 1;
        unsigned	pbtbackward : 1;
        DLword  dummy[4];	/* unused, needed to make 16-alignment */
} TEXTUREBBT;


#endif /* BYTESWAP */

