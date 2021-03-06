/* graph.h - graphics functions
 * $Version: 1.11 $
 * Copyright 1990,91 Intel Corporation, ALL RIGHTS RESERVED.
 */

#ifndef _graphh
#define _graphh

#ifndef _FAR
#define _FAR
#define _HUGE
#define _NEAR
#endif

/*
 * Structure returned by _getvideoconfig():
 */
#ifndef _videoconfig
struct videoconfig {
    short numxpixels;                   /* Number of pixels on X axis        */
    short numypixels;                   /* Number of pixels on Y axis        */
    short numtextcols;                  /* Number of text columns available  */
    short numtextrows;                  /* Number of text rows available     */
    short numcolors;                    /* Number of actual colors           */
    short bitsperpixel;                 /* Number of bits per pixel          */
    short numvideopages;                /* Number of available video pages   */
    short mode;                         /* Current video mode                */
    short adapter;                      /* Active display adapter            */
    short monitor;                      /* Active display monitor            */
    short memory;                       /* Adapter video memory in K bytes   */
};
#define _videoconfig
#define _VIDEOCONFIG_DEFINED
#endif

/*
 * X-/Y-coordinate structure returned by _setvieworg(), et al.:
 */
#ifndef _xycoord
struct xycoord {
   short xcoord;
   short ycoord;
};
#define _xycoord
#define _XYCOORD_DEFINED
#endif

/*
 * Row/Column position structure for text:
 */
#ifndef _rccoord
struct rccoord {
   short row;
   short col;
};
#define _rccoord
#define _RCCOORD_DEFINED
#endif

/*
 * Window coordinate structure:
 */
#ifndef __wxycoord
struct _wxycoord {
   double wx;                                         /* Window X coordinate */
   double wy;                                         /* Window Y coordinate */
   };
#define __wxycoord
#define _WXYCOORD_DEFINED
#endif

/*
 * Structure returned by _getfontinfo():
 */
#ifndef __fontinfo
struct _fontinfo {
    int     type;               /* Bit 0 set = vector, clear = bit map       */
    int     ascent;             /* Pixel distance from top to baseline       */
    int     pixwidth;           /* Character width in pixels, 0=proportional */
    int     pixheight;          /* Character height in pixels                */
    int     avgwidth;           /* Average character width in pixels         */
    char    filename[81];       /* File name including path                  */
    char    facename[32];       /* Font name                                 */
};
#define __fontinfo
#define _FONTINFO_DEFINED
#endif

/*
 * _grstatus() status values:
 */
#define _GROK                        0                            /* Success */
#define _GRNOOUTPUT                  1                           /* Warnings */
#define _GRCLIPPED                   2
#define _GRPARAMETERALTERED          3
#define _GRERROR                   (-1)                            /* Errors */
#define _GRMODENOTSUPPORTED        (-2)
#define _GRNOTINPROPERMODE         (-3)
#define _GRINVALIDPARAMETER        (-4)
#define _GRFONTFILENOTFOUND        (-5)
#define _GRINVALIDFONTFILE         (-6)
#define _GRCORRUPTEDFONTFILE       (-7)
#define _GRINSUFFICIENTMEMORY      (-8)
#define _GRINVALIDIMAGEBUFFER      (-9)


/*
 * Arguments to _setvideomode():
 */
#define _MAXRESMODE    (-3)         /* Graphics mode with highest resolution */
#define _MAXCOLORMODE  (-2)         /* Graphics mode with most colors        */
#define _DEFAULTMODE   (-1)         /* Restore screen to original mode       */
#define _TEXTBW40        0          /* 40-column text, 16 grey               */
#define _TEXTC40         1          /* 40-column text, 16/8 color            */
#define _TEXTBW80        2          /* 80-column text, 16 grey               */
#define _TEXTC80         3          /* 80-column text, 16/8 color            */
#define _MRES4COLOR      4          /* 320 x 200, 4 color                    */
#define _MRESNOCOLOR     5          /* 320 x 200, 4 grey                     */
#define _HRESBW          6          /* 640 x 200, black & white              */
#define _TEXTMONO        7          /* 80-column text, black & white         */
#define _HERCMONO        8          /* 720 x 348, black and white for        */
                                    /*  Hercules graphics card               */
#define _MRES16COLOR    13          /* 320 x 200, 16 color                   */
#define _HRES16COLOR    14          /* 640 x 200, 16 color                   */
#define _ERESNOCOLOR    15          /* 640 x 350, black & white              */
#define _ERESCOLOR      16          /* 640 x 350, 4 or 16 color              */
#define _VRES2COLOR     17          /* 640 x 480, black & white              */
#define _VRES16COLOR    18          /* 640 x 480, 16 color                   */
#define _MRES256COLOR   19          /* 320 x 200, 256 color                  */
#define _ORESCOLOR      64          /* 640 x 400, 1 of 16 color              */

/*
 * Adapter values used with _getvideoconfig() and struct videoconfig:
 */
#define _EGA            0x0004      /* EGA - Enhanced Graphics Adapter       */
#define _VGA            0x0008      /* VGA - Video Graphics Array            */

/*
 * Monitor values used with _getvideoconfig() and struct videoconfig:
 */
#define _MONO           0x0001      /* Monochrome                            */
#define _COLOR          0x0002      /* Color (or enhanced emulating color)   */
#define _ENHCOLOR       0x0004      /* Enhanced Color                        */
#define _ANALOGMONO     0x0008      /* Analog monochrome only                */
#define _ANALOGCOLOR    0x0010      /* Analog color only                     */
#define _ANALOG         0x0018      /* Analog monochrome and color modes     */

/*
 * Outline/Fill values used with _ellipse, _rectangle, _pie, and _polygon:
 */
#define _GBORDER        0x02        /* Draw only the outline                 */
#define _GFILLINTERIOR  0x03        /* Fill using the current fill mask      */

/*
 * _clearscreen operations:
 */
#define _GCLEARSCREEN   0x00
#define _GVIEWPORT      0x01
#define _GWINDOW        0x02

/*
 * _displaycursor operations:
 */
#define _GCURSOROFF     0x00
#define _GCURSORON      0x01

/*
 * _wrapon operations:
 */
#define _GWRAPOFF       0x00
#define _GWRAPON        0x01

/*
 * Scroll directions used with _scrolltextwindow:
 */
#define _GSCROLLUP      0x01
#define _GSCROLLDOWN   (-1)

/*
 * Non-specific "maximum number of rows" used with _settextrows and
 * _setvideomoderows:
 */
#define _MAXTEXTROWS   (-1)

/*
 * _putimage() and _setwritemode() operations:
 */
#define _GOR            0x00
#define _GAND           0x01
#define _GPRESET        0x02
#define _GPSET          0x03
#define _GXOR           0x04

/*
 * Color values (palette colors) valid in all color modes,
 * used with _setbkcolor, _remappalette, and _remapallpalette:
 */
#define _BLACK          0x000000L
#define _RED            0x00002AL
#define _BROWN          0x00152AL
#define _GREEN          0x002A00L
#define _GRAY           0x151515L
#define _LIGHTRED       0x15153FL
#define _LIGHTGREEN     0x153F15L
#define _YELLOW         0x153F3FL
#define _BLUE           0x2A0000L
#define _MAGENTA        0x2A002AL
#define _CYAN           0x2A2A00L
#define _WHITE          0x2A2A2AL
#define _LIGHTBLUE      0x3F1515L
#define _LIGHTMAGENTA   0x3F153FL
#define _LIGHTCYAN      0x3F3F15L
#define _BRIGHTWHITE    0x3F3F3FL
#define _LIGHTYELLOW    _YELLOW    /* Obsolete */

/*
 * Color values used with _ERESNOCOLOR (Monochrome mode 0F):
 */
#define _MODEFOFF       0x00L
#define _MODEFOFFTOON   0x01L
#define _MODEFOFFTOHI   0x02L
#define _MODEFONTOOFF   0x03L
#define _MODEFON        0x04L
#define _MODEFONTOHI    0x05L
#define _MODEFHITOOFF   0x06L
#define _MODEFHITOON    0x07L
#define _MODEFHI        0x08L

/*
 * Color values used with _TEXTMONO (Monochrome mode 7):
 */
#define _MODE7OFF       0x00L
#define _MODE7ON        0x01L
#define _MODE7HI        0x02L

/*
 * Function prototypes:
 */
short          _arc(short, short, short, short,
                  short, short, short, short);
short          _arc_w(double, double, double, double,
                  double, double, double, double);
short          _arc_wxy(const struct _wxycoord *, const struct _wxycoord *,
                  const struct _wxycoord *, const struct _wxycoord *);
void           _clearscreen(short);
short          _displaycursor(short);
short          _ellipse(short, short, short, short, short);
short          _ellipse_w(short, double, double, double, double);
short          _ellipse_wxy(short, const struct _wxycoord *,
                  const struct _wxycoord *);
short          _floodfill(short, short, short);
short          _floodfill_w(double, double, short);
short          _getactivepage(void);
short          _getarcinfo(struct xycoord *, struct xycoord *,
                  struct xycoord *);
long           _getbkcolor(void);
short          _getcolor(void);
struct xycoord _getcurrentposition(void);
struct _wxycoord _getcurrentposition_w(void);
unsigned char *_getfillmask(unsigned char *);
short          _getfontinfo(struct _fontinfo *);
short          _getgtextextent(const unsigned char *);
struct xycoord _getgtextvector(void);
void           _getimage(short, short, short, short, char _HUGE *);
void           _getimage_w(double, double, double, double, char _HUGE *);
void           _getimage_wxy(const struct _wxycoord *,
                  const struct _wxycoord *, char _HUGE *);
unsigned short _getlinestyle(void);
struct xycoord _getphyscoord(short, short);
short          _getpixel(short, short);
short          _getpixel_w(double, double);
short          _gettextcolor(void);
short          _gettextcursor(void);
struct rccoord _gettextposition(void);
void           _gettextwindow(short *, short *, short *, short *);
struct videoconfig *_getvideoconfig(struct videoconfig *);
struct xycoord _getviewcoord(short, short);
struct xycoord _getviewcoord_w(double,double);
struct xycoord _getviewcoord_wxy(const struct _wxycoord *);
short          _getvisualpage(void);
struct _wxycoord _getwindowcoord(short,short);
struct _wxycoord _getwindowcoord_xy(struct xycoord);
short          _getwritemode(void);
short          _grstatus(void);
long           _imagesize(short, short, short, short);
long           _imagesize_w(double, double, double, double);
long           _imagesize_wxy(const struct _wxycoord *,
                  const struct _wxycoord *);
short          _lineto(short, short);
short          _lineto_w(double, double);
struct xycoord _moveto(short, short);
struct _wxycoord _moveto_w(double, double);
void           _outgtext(const unsigned char *);
void           _outmem(const unsigned char *, short);
void           _outtext(const unsigned char *);
short          _pie(short, short, short, short, short,
                  short, short, short, short);
short          _pie_w(short, double, double, double, double,
                  double, double, double, double);
short          _pie_wxy(short, const struct _wxycoord *,
                  const struct _wxycoord *, const struct _wxycoord *,
                  const struct _wxycoord *);
short          _polygon(short, const struct xycoord *, short);
short          _polygon_w(short, const double *, short);
short          _polygon_wxy(short, const struct _wxycoord *, short);
void           _putimage(short, short, char _HUGE *, short);
void           _putimage_w(double, double ,char _HUGE * ,short);
short          _rectangle(short, short, short, short, short);
short          _rectangle_w(short, double, double, double, double);
short          _rectangle_wxy(short, const struct _wxycoord *,
                  const struct _wxycoord *);
short          _registerfonts(const unsigned char *);
short          _remapallpalette(const long *);
long           _remappalette(short, long);
void           _scrolltextwindow(short);
short          _selectpalette(short);
short          _setactivepage(short);
long           _setbkcolor(long);
void           _setcliprgn(short, short, short, short);
short          _setcolor(short);
void           _setfillmask(const unsigned char *);
short          _setfont(const unsigned char *);
struct xycoord _setgtextvector( short, short);
void           _setlinestyle(unsigned short);
short          _setpixel(short, short);
short          _setpixel_w(double, double);
short          _settextcolor(short);
short          _settextcursor(short);
struct rccoord _settextposition(short, short);
short          _settextrows(short);
void           _settextwindow(short, short, short, short);
short          _setvideomode(short);
short          _setvideomoderows(short,short);
struct xycoord _setvieworg(short, short);
void           _setviewport(short, short, short, short);
short          _setvisualpage(short);
short          _setwindow(short,double,double,double,double);
short          _setwritemode(short);
void           _unregisterfonts(void);
short          _wrapon(short);

/*
 * Obsolete function names:
 */
#define _getlogcoord _getviewcoord
#define _setlogorg   _setvieworg

#endif /* _graphh */
