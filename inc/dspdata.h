/* @(#) dspdata.h Version 1.1 (4/21/92). copyright Venue & Fuji Xerox  */
/* This is G-file @(#) dspdata.h Version 1.1 (4/21/92). copyright Venue & Fuji Xerox  */




/************************************************************************/
/*									*/
/*	Copyright 1989, 1990 Venue, Fuji Xerox Co., Ltd, Xerox Corp.	*/
/*									*/
/*	This file is work-product resulting from the Xerox/Venue	*/
/*	Agreement dated 18-August-1989 for support of Medley.		*/
/*									*/
/************************************************************************/




#ifndef BYTESWAP
	/******************************************************/
	/* Normal-byte-order declarations, for, e.g., 68020's */
	/******************************************************/
typedef struct	displaydata{

     LispPTR	ddxposition ;
     LispPTR	ddyposition;
     LispPTR	ddxoffset;
     LispPTR	ddyoffset;
     LispPTR	dddestination;
     LispPTR	ddclippingregion;
     LispPTR	ddfont;
     LispPTR	ddslowprintingcase;
     LispPTR	ddwidthscache;
     LispPTR	ddoffsetscache;
     LispPTR	ddcolor;
     LispPTR	ddlinefeed;
     LispPTR	ddrightmargin;
     LispPTR	ddleftmargin;
     LispPTR	ddscroll;
     LispPTR	ddoperation;
     unsigned   ddheldflg : 1;
     unsigned   nil1 : 7;
     unsigned	ddsourcetype : 24;

     DLword	ddclippingleft;
     DLword	ddclippingright;
     DLword	ddclippingbottom;
     DLword	ddclippingtop;
     DLword	nil2;
     DLword	ddcharsetascent; 
     LispPTR	xwindowhint	; 	/* xpointer  */
     LispPTR	ddpilotbbt;
     LispPTR	ddxscale;
     LispPTR	ddyscale;
     LispPTR	ddcharimagewidths;/* Lisp POINTER to DLword array (49,50)*/
     LispPTR	ddeolfn;
     LispPTR	ddpagefullfn;
     LispPTR	ddtexture;
     LispPTR	ddmicaxpos;
     LispPTR	ddmicaypos;
     LispPTR	ddmicarightmargin;
     LispPTR	ddcharset;
     DLword	ddcharsetdescent;
     DLword	ddspacewidth;  /* ??*/
     LispPTR	ddcharheightdelta; /* NUM PTR */
}DISPLAYDATA;

#else
	/*************************************************/
	/* Byte-swapped declarations, for, e.g., 80386's */
	/*************************************************/
typedef struct	displaydata{

     LispPTR	ddxposition ;
     LispPTR	ddyposition;
     LispPTR	ddxoffset;
     LispPTR	ddyoffset;
     LispPTR	dddestination;
     LispPTR	ddclippingregion;
     LispPTR	ddfont;
     LispPTR	ddslowprintingcase;
     LispPTR	ddwidthscache;
     LispPTR	ddoffsetscache;
     LispPTR	ddcolor;
     LispPTR	ddlinefeed;
     LispPTR	ddrightmargin;
     LispPTR	ddleftmargin;
     LispPTR	ddscroll;
     LispPTR	ddoperation;
     unsigned	ddsourcetype : 24;
     unsigned   nil1 : 7;
     unsigned   ddheldflg : 1;

     DLword	ddclippingright;
     DLword	ddclippingleft;
     DLword	ddclippingtop;
     DLword	ddclippingbottom;
     DLword	ddcharsetascent; 
     DLword	nil2;
 
     LispPTR	xwindowhint	; 	/* xpointer  */
     LispPTR	ddpilotbbt;
     LispPTR	ddxscale;
     LispPTR	ddyscale;
     LispPTR	ddcharimagewidths;/* Lisp POINTER to DLword array (49,50)*/
     LispPTR	ddeolfn;
     LispPTR	ddpagefullfn;
     LispPTR	ddtexture;
     LispPTR	ddmicaxpos;
     LispPTR	ddmicaypos;
     LispPTR	ddmicarightmargin;
     LispPTR	ddcharset;
     DLword	ddspacewidth;  /* ??*/
     DLword	ddcharsetdescent;
     LispPTR	ddcharheightdelta; /* NUM PTR */
}DISPLAYDATA;

#endif /* BYTESWAP */

