/* @(#) lldsp.h Version 1.1 (4/21/92). copyright Venue & Fuji Xerox  */
/* @(#) lldsp.h Version 1.1 (4/21/92). copyright Venue & Fuji Xerox  */
/***************************************************************/
/*
		lldsp.h
		By Takeshi Shimizu

		typedef for 
			FONTDESC,CHARSETINFO,SCREEN,WINDOW

*/
/***************************************************************/
#ifndef BYTESWAP

typedef struct {
		LispPTR		FONTDEVICE;
		LispPTR		SFObsolete1;
		LispPTR		FONTFAMILY;
		LispPTR		FONTSIZE;
		LispPTR		FONTFACE;
		LispPTR		SFObsolete2;
		LispPTR		SFObsolete3;
		LispPTR		SFObsolete4;
		DLword		SFObsolete5;
		DLword		SFObsolete6;
		DLword		SFAscent;
		DLword		SFDescent;
		DLword		SFHeight;
		DLword		ROTATION;
		short		FBBOX;
		short		FBBOY;
		short		FBBDX;
		short		FBBDY;
		unsigned	SFFACECODE		: 8;
		unsigned	SFLKerns		:24;
		LispPTR		SFRWidths;
		LispPTR		FONTDEVICESPEC;
		LispPTR		OTHERDEVICEFONTPROPS;
		LispPTR		FONTSCALE;
		DLword		FONTAVGCHARWIDTH;
		DLword		dum;
		LispPTR		FONTIMAGEWIDTHS;
		LispPTR		FONTCHARSETVECTOR;
		LispPTR		FONTEXTRAFIELD2;
} FONTDESC;

typedef struct {
		LispPTR	WIDTHS;
		LispPTR	OFFSETS;
		LispPTR	IMAGEWIDTHS;
		LispPTR	CHARSETBITMAP;
		LispPTR	YWIDTHS;
		DLword	CHARSETASCENT;
		DLword	CHARSETDESCENT;
		LispPTR	LEFTKERN;
 } CHARSETINFO;

#else /* BYTESWAP */
typedef struct {
		LispPTR		FONTDEVICE;
		LispPTR		SFObsolete1;
		LispPTR		FONTFAMILY;
		LispPTR		FONTSIZE;
		LispPTR		FONTFACE;
		LispPTR		SFObsolete2;
		LispPTR		SFObsolete3;
		LispPTR		SFObsolete4;
		DLword		SFObsolete6;
		DLword		SFObsolete5;
		DLword		SFDscent;
		DLword		SFAescent;
		DLword		ROTATION;
		DLword		SFHeight;
		short		FBBOY;
		short		FBBOX;
		short		FBBDY;
		short		FBBDX;
		unsigned	SFLKerns		:24;
		unsigned	SFFACECODE		: 8;
		LispPTR		SFRWidths;
		LispPTR		FONTDEVICESPEC;
		LispPTR		OTHERDEVICEFONTPROPS;
		LispPTR		FONTSCALE;
		DLword		dum;
		DLword		FONTAVGCHARWIDTH;
		LispPTR		FONTIMAGEWIDTHS;
		LispPTR		FONTCHARSETVECTOR;
		LispPTR		FONTEXTRAFIELD2;
} FONTDESC;

typedef struct {
		LispPTR	WIDTHS;
		LispPTR	OFFSETS;
		LispPTR	IMAGEWIDTHS;
		LispPTR	CHARSETBITMAP;
		LispPTR	YWIDTHS;
		DLword	CHARSETDESCENT;
		DLword	CHARSETASCENT;
		LispPTR	LEFTKERN;
 } CHARSETINFO;

#endif /* BYTESWAP */



typedef struct {
		LispPTR SCONOFF;
		LispPTR SCDESTINATION;
		LispPTR SCWIDTH;
		LispPTR SCHEIGHT;
		LispPTR SCTOPW;
		LispPTR SCTOPWDS;
		LispPTR SCTITLEDS;
		LispPTR SCFDEV;
		LispPTR SCDS;
		LispPTR SCDATA;
 } SCREEN;
typedef struct {
		LispPTR DSP;
		LispPTR NEXTW;
		LispPTR SAVE;
		LispPTR REG;
		LispPTR BUTTONEVENTFN;
		LispPTR RIGHTBUTTONFN;
		LispPTR CURSORINFN;
		LispPTR CURSOROUTFN;
		LispPTR CURSORMOVEFN;
		LispPTR REPAINTFN;
		LispPTR RESHAPEFN;
		LispPTR EXTENT;
		LispPTR USERDATA;
		LispPTR VERTSCROLLREG;
		LispPTR HORIZSCROLLREG;
		LispPTR SCROLLFN;
		LispPTR VERTSCROLLWINDOW;
		LispPTR HORIZSCROLLWINDOW;
		LispPTR CLOSEFN;
		LispPTR MOVEFN;
		LispPTR WTITLE;
		LispPTR NEWREGION;
		LispPTR WBORDER;
		LispPTR PROCESS;
		LispPTR WINDOWENTRYFN;
		LispPTR SCREEN;
} WINDOW;
