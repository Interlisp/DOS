typedef void (*PFV)();		/* Pointer to Function returning Void */
typedef int (*PFI)();		/* Pointer to Function returning Int */
typedef char (*PFC)();		/* Pointer to Function returning Char */
typedef float (*PFF)();		/* Pointer to Function returning Float */
typedef int (*PFP)();		/* Pointer to Function returning a Pointer */
typedef int (*PFUL)();		/* Pointer to Function returning an unsigned long */

#ifndef TRUE
#define FALSE 0
#define TRUE  !FALSE
#endif /* TRUE */

#ifdef XWINDOW
#include <X11/Xlib.h>
#endif /* XWINDOW */

typedef struct {
  void			(* enter_display)(); /* Init routine for dsp. */
  void			(* exit_display)(); /* Exit routine for dsp. */

  void			(* before_raid)();
  void			(* after_raid)();
  
  void			(* drawline)();

  void			(* cleardisplay)();

  unsigned long		(* get_color_map_entry)();
  unsigned long		(* set_color_map_entry)();
  unsigned long		(* available_colors)();	/* How many colors do I have on my palette */
  unsigned long		(* possible_colors)(); /* How many colors is it possible to select from */

#ifdef NOTYET
  unsigned long		(* get_color_map)(); /* get a pointer to a colormap */
  unsigned long		(* set_color_map)(); /* set the current colormap */
  unsigned long		(* make_color_map)(); /* return a brand new colormap */
#endif /* NOTYET */

  unsigned long		(* medley_to_native_bm)(); /* 1 bit/pix to native bit/pix */
  unsigned long		(* native_to_medley_bm)(); /* native bit/pix to 1 bit/pix */

  unsigned long		(* bitblt_to_screen)();
  unsigned long		(* bitblt_from_screen)();
  unsigned long		(* scroll_region)(); /* ie. bbt from screen to screen */

  unsigned long		displaywidth;
  unsigned long		displayheight;
  unsigned short	bitsperpixel;
  unsigned long		colors;	/* cash for the available_colors */
  unsigned long		*oldstate; /* Keep the old state around */
  unsigned long		graphicsmode; /* Magic cookie used to set the state. */
  unsigned long		numberofbanks;
#ifdef DOS
  unsigned long		BytesPerLine;
  unsigned long		DisplayStartAddr;
#elif XWINDOW
  char			*identifier;
  int			BitGravity;
  Display	*display_id;
  Window	LispWindow;
  Window	DisplayWindow;
  Window	HorScrollBar;
  Window	VerScrollBar;
  Window	HorScrollButton;
  Window	VerScrollButton;
  Window	NEGrav;
  Window	SEGrav;
  Window	SWGrav;
  Window	NWGrav;
  GC		Copy_GC;
  unsigned int	UpperLeftX;	/* Vissible part of the Medley display */
  unsigned int	UpperLeftY;	/* Vissible part of the Medley display */
  unsigned int	VissibleWidth;	/* Vissible part of the Medley display */
  unsigned int	VissibleHeight;	/* Vissible part of the Medley display */
  unsigned int	InternalBorderWidth;
  unsigned int	ScrollBarWidth;
  Pixmap	ScrollBarPixmap;
  Pixmap	GravityOnPixmap;
  Pixmap	GravityOffPixmap;
  XImage	ScreenBitmap;
  Mask		DisableEventMask;
  Mask		EnableEventMask;
#endif /* XWINDOW */
} DspInterfaceRec, *DspInterface;

/* ENTER_DISPLAY
 args: self
 Called to set up the display. Has to be called before anything else is done to
 the display.
 */

/* EXIT_DISPLAY
 args: self
 Called to take down the display and restore the old state of the display.
 */

/* BEFORE_RAID
 args: self
 Called before raid is entered. Sets up the display for raid.
 */

/* AFTER_RAID
 args: self
 Called to return from raid to lisp.
 */

/* MEDLEY_TO_NATIVE_BM
 args: self, medley_bitmap_pointer, native_bitmap_pointer
 Called to convert a bitmap from lisps 1 bit per pixel to the
 native format. Think of it as colorize bitmap.
 */

/* NATIVE_TO_MEDLEY_BM
 args: self, medley_bitmap_pointer, native_bitmap_pointer
 Called to convert a native bitmap to lisps 1 bit per pixel format.
 Think of this as uncolorize bitmap.
 */

/* DRAWLINE
 args: self, startX, startY, width, height, function, color,
       thickness, butt, clipX, clipY, clipWidth, clipHeight, dashing
 function: 0=set, 1=reset, 2=invert, 3=paint.
 color: an index into the colormap
 thickness: how many pixels wide is a line.
 butt: 0=medley-style, 1=cap, 2=bevel, 3=mitre.
 dashing: a pointer to a lisp list of dashings.
 clip<XXX>: The line has a thickness so we have to supply
       information on how to clip the butts of the line.
 */

/* GET_COLOR_MAP
 args: self
 Returns the current colormap
 */

/* SET_COLOR_MAP
 args: self, colormap.
 */

/* MAKE_COLOR_MAP
 args: self
 Return a new instance of colormap
 */

/* GET_COLOR_MAP_ENTRY
 args: self, colormap, index
 */

/* SET_COLOR_MAP_ENTRY
 args: self, colormap, index, newcolor
 */

/* BITBLT_TO_SCREEN
 args: self, buffer left top width height */

/* CLEARSCREEN
 args: self
 */

#define DefineCursor(display, window, mycursor) { \
	XLOCK;\
	XDefineCursor( display, window, *(mycursor) );\
	XUNLOCK;\
}


#define OUTER_SB_WIDTH(dsp) (dsp->ScrollBarWidth + 2*(dsp->InternalBorderWidth))

#define min( a, b ) (((a)<(b))?(a):(b))
#define max( a, b ) (((a)>(b))?(a):(b))
#define mid(a, b, c) max( min( b, max( a, c ), min( a, max( b, c ))))
