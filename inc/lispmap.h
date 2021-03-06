/* @(#) lispmap.h Version 2.6 (4/21/92). copyright Venue & Fuji Xerox  */

/*
 *	Copyright (C) 1987 by Fuji Xerox Co., Ltd. All rights reserved.
 *
 *	Author	:	Takeshi Shimizu
 *			Hiroshi Hayata
 */
/**********************************************************************/
/*
		File Name :	lispmap.h(for TEST)

		**************NOTE*****************
		OLD DEFs are MOVED to lispmap.FULL
		**************NOTE*****************

		Global variables for LispSYSOUT

					Date :		December 18, 1986
					Edited by :	Takeshi Shimizu

*/
/**********************************************************************/



/************************************************************************/
/*									*/
/*	Copyright 1989, 1990 Venue, Fuji Xerox Co., Ltd, Xerox Corp.	*/
/*									*/
/*	This file is work-product resulting from the Xerox/Venue	*/
/*	Agreement dated 18-August-1989 for support of Medley.		*/
/*									*/
/************************************************************************/




 /* Whole Lisp size */
#define LWORLD_SIZE		0x420000	/* byte */

/* 1 MDS entry size is 2(page) * 512 Byte */
#define MDSTT_SIZE		(LWORLD_SIZE >> 10 )

/* if you want to use the ATOMSPACE for Dummy then it must be 0x10000 take */
#define MAP_SHIFT	0x0

/* Following constants mean LISP word offset. */
/* these correspond with lisp mem map */
/* for IOCBPAGE */
#define IOCBPAGE_OFFSET	    256
#define IOCB_SIZE	      1

/* for ATOMSPACE */
#define ATOMS_HI	      0
#define ATOMS_OFFSET	0x00000
#define ATOMS_SIZE	0x10000

/* for IOPAGE */
#define IOPAGE_OFFSET	0x0FF00
#define IOPAGE_SIZE	      1

/* for STACKSPACE */
#define STK_HI		      1
#define STK_OFFSET	0x10000
#define STK_SIZE	0x10000

/* for PLISTSPACE */
#define PLIS_HI		      2
#define PLIS_OFFSET	0x20000
#define PLIS_SIZE	0x20000

/* for FPTOVP */
#define FPTOVP_HI	      4
#define FPTOVP_OFFSET	0x40000
#define FPTOVP_SIZE	0x10000

/*for PAGEMAP */
#define PAGEMAP_HI	      5
#define PAGEMAP_OFFSET	0x50000
#define PAGEMAP_SIZE	0x10000

/* for InterfacePage */
#define IFPAGE_HI	      6
#define IFPAGE_OFFSET	0x60000
#define IFPAGE_SIZE	  0x200

/* for PageMapTBL */
#define PAGEMAPTBL_OFFSET 0x60200
#define PAGEMAPTBL_SIZE	    0x800

/* for MISCSTATS */
#define MISCSTATS_OFFSET  0x60A00
#define MISCSTATS_SIZE	    0x200

/* for UFNTable */
#define UFNTBL_OFFSET	0x60C00
#define UFNTBL_SIZE	  0x200

/* for DTDspace */
#define DTD_HI		      6
#define DTD_OFFSET	0x61000
#define DTD_SIZE	 0x1000

/* for LOCKEDPAGETBL */
#define LOCKEDPAGETBL_OFFSET 0x67000
#define LOCKEDPAGETBL_SIZE 0x1000

/* for MDSTT */
#define MDS_HI		      6
#define MDS_OFFSET	0x68000
#define MDS_SIZE	 0x8000

/* for AtomHashTable */
#define ATMHT_HI	      7
#define ATMHT_OFFSET	0x70000
#define ATMHT_SIZE	0x10000

/* for PNPSPACE */
#define PNP_HI		      8
#define PNP_OFFSET	0x80000
#define PNP_SIZE	0x20000

/* for DEFSPACE */
#define DEFS_HI		     10
#define DEFS_OFFSET	0xA0000
#define DEFS_SIZE	0x20000

/* for VALSPACE */
#define VALS_HI		     12
#define VALS_OFFSET	0xC0000
#define VALS_SIZE	0x20000

/* for Small Positive */
#define SPOS_HI		     14
#define S_POSITIVE	0xE0000
#define SPOS_SIZE	0x10000

/* for Small Negative */
#define SNEG_HI		     15
#define S_NEGATIVE	0xF0000
#define SNEG_SIZE	0x10000

/* for characters */
#define S_CHAR		0x70000

/* for HTMAIN */
#define HTMAIN_HI	     16
#define HTMAIN_OFFSET  0x100000
#define HTMAIN_SIZE	 0x8000

/* for HTOVERFLOW */
#define HTOVERFLOW_OFFSET 0x108000
#define HTOVERFLOW_SIZE	     0x100

/* for HTBIGCOUNT */
#define HTBIG_HI	     16
#define HTBIG_OFFSET   0x108100
#define HTBIG_SIZE       0x8000

/* for HTCOLL */
#define HTCOLL_HI	     17
#define HTCOLL_OFFSET  0x110000
#define HTCOLL_SIZE	0x10000

/* DISPLAYREGION */

#define DISPLAY_HI	     18
#define DISPLAY_OFFSET 0x120000


#ifdef MEDLEY
/* for ARRAYSPACE & MDS  for PROT-LISP */
#define ARRAY_OFFSET		0x140000
#else
#define ARRAY_OFFSET		0x130000
#endif

#define MDS_BOTTOM_OFFSET	0x200000

/* for PnCharSpace(use only PROT-LISP ) */
#define PNCHAR_HI	   0x20
#define PNCHAR_OFFSET  0x200000
#define PNCHAR_SIZE	0x10000


/***** SEG definitions for AtomCellN *****/
/* following defs correspond with D machine memory layout */
/**** NOTE!!  if D's layout changes, modify following defs */
#define D_PLISHI	2
#define D_PNHI		010
#define D_DEFSHI	012
#define D_VALSHI	014
