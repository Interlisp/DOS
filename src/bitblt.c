/* @(#) bitblt.c Version 2.26 (4/21/92). copyright Venue & Fuji Xerox  */
static char *id = "@(#) bitblt.c        2.26 4/21/92            (envos & Fuji Xerox)";


/*
 *
 *      Author :  Osamu Nakamura
 *
 */
#include <stdio.h>

#ifndef NOPIXRECT
#ifndef DOS
#include <sunwindow/window_hs.h>
#include <sunwindow/win_ioctl.h>

#include <suntool/sunview.h>
#include <sunwindow/cms_mono.h>
#include <suntool/canvas.h>
#include <sys/ioctl.h>
#endif /* DOS */
#include <signal.h>
#endif

#ifdef XWINDOW 
#define DISPLAYBUFFER
#endif /* XWINDOW */


#include "lispemul.h"
#include "lspglob.h"
#include "lispmap.h"
#include "emlglob.h"
#include "adr68k.h"
#include "address.h"

#include "pilotbbt.h"
#include "display.h"
#include "bitblt.h"
#include "bb.h"

extern int LispWindowFd;
extern int ScreenLocked;
extern int kbd_for_makeinit;

#ifdef COLOR
extern int MonoOrColor;
#endif /* COLOR */


/*****************************************************************************/
/**                                                                         **/
/**                             N_OP_pilotbitblt                            **/
/**                                                                         **/
/**       The Native-code compatible version of the opcode for bitblt.      **/
/**                                                                         **/
/**                                                                         **/
/*****************************************************************************/


LispPTR N_OP_pilotbitblt(pilot_bt_tbl, tos)
  LispPTR pilot_bt_tbl;
  int tos;
  {
    PILOTBBT  *pbt;
    DLword      *srcbase, *dstbase;
    int displayflg;
    int sx, dx, w, h, srcbpl, dstbpl, backwardflg;
    int src_comp, op, gray, num_gray, curr_gray_line;

#ifdef INIT

        /* for init, we have to initialize the pointers at the
           first call to pilotbitblt or we die.  If we do it 
           earlier we die also.  We set a new flag so we don't
           do it more than once which is a lose also. 

           I put this in an ifdef so there won't be any extra
           code when making a regular LDE.  */
        
    if (!kbd_for_makeinit)
      {
        init_keyboard(0);
        kbd_for_makeinit = 1;
      };


#endif

    pbt = (PILOTBBT *)Addr68k_from_LADDR(pilot_bt_tbl);

    w  = pbt->pbtwidth;
    h  = pbt->pbtheight;
    if ((h <= 0) || (w <= 0)) return(pilot_bt_tbl);     
    dx = pbt->pbtdestbit;
    sx = pbt->pbtsourcebit;
    backwardflg = pbt->pbtbackward;
    /* if displayflg != 0 then source or destination is DisplayBitMap */
    ScreenLocked=T;

#ifdef SUNDISPLAY
    displayflg = cursorin(pbt->pbtdesthi, (pbt->pbtdestlo + (dx >> 4)),
                              w, h, backwardflg)  ||
                 cursorin(pbt->pbtsourcehi, (pbt->pbtsourcelo + (sx >> 4)), 
                              w, h, backwardflg); 
#endif /* SUNDISPLAY */


    srcbase = (DLword *)Addr68k_from_LADDR( VAG2( pbt->pbtsourcehi,
                                             pbt->pbtsourcelo));
    dstbase = (DLword *)Addr68k_from_LADDR( VAG2( pbt->pbtdesthi, 
                                              pbt->pbtdestlo ));

    srcbpl   = pbt->pbtsourcebpl;
    dstbpl   = pbt->pbtdestbpl;
    src_comp = pbt->pbtsourcetype;
    op       = pbt->pbtoperation;
    gray     = pbt->pbtusegray;
    num_gray       = ((TEXTUREBBT *)pbt)->pbtgrayheightlessone + 1;
    curr_gray_line = ((TEXTUREBBT *)pbt)->pbtgrayoffset;

#ifdef SUNDISPLAY
    if( displayflg ) HideCursor;
#endif /* SUNDISPLAY */


    new_bitblt_code

#ifdef SUNDISPLAY
#ifdef DISPLAYBUFFER
#ifdef COLOR
    if( MonoOrColor == MONO_SCREEN )
#endif /* COLOR */

    if (in_display_segment(dstbase))
        flush_display_lineregion(dx, dstbase, w, h);
#endif
    if( displayflg) ShowCursor;
#endif /* SUNDISPLAY */


#ifdef XWINDOW
    flush_display_lineregion(dx, dstbase, w, h);
#endif /* XWINDOW */


    ScreenLocked=NIL;

    return(pilot_bt_tbl);

  } /* end of N_OP_pilotbitblt */



/************************************************************************/
/*                                                                      */
/*                              c u r s o r i n                                 */
/*                                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/

#ifndef COLOR
/* for MONO only */
int cursorin (addrhi, addrlo, w, h, backward)
  DLword        addrhi;                 /* Lisp addr hi-word    */
  DLword        addrlo;                 /* Lisp addr lo-word    */
  register int  w, h;
  {
     register int       x, y;
    if (addrhi == DISPLAY_HI)
      {
        y = addrlo / DisplayRasterWidth;
        x = (addrlo - y * DisplayRasterWidth) << 4;
      }
    else if (addrhi == DISPLAY_HI+1)
      {
        y = (addrlo + DLWORDSPER_SEGMENT) / DisplayRasterWidth;
        x = ((addrlo + DLWORDSPER_SEGMENT) - y * DisplayRasterWidth) << 4;
      }
    else return( NIL );

    if(backward) y -= h;
                
    if((x < MOUSEXR)&&(x + w > MOUSEXL)&&(y < MOUSEYH)&&(y + h > MOUSEYL))
           return( T );
    else return( NIL );
  }
#else

/* for COLOR & MONO */
int cursorin (addrhi, addrlo, w, h, backward)
     DLword     addrhi;                 /* Lisp addr hi-word    */
     DLword     addrlo;                 /* Lisp addr lo-word    */
     register int       w, h;
     {
     register int       x, y;
     register DLword *base68k;
    extern int MonoOrColor;
    extern int displaywidth;
    extern DLword *ColorDisplayRegion68k;

 if(MonoOrColor == MONO_SCREEN) { /* On MONO screen */
        if (addrhi == DISPLAY_HI)
        {
          y = addrlo / DisplayRasterWidth;
          x = (addrlo - y * DisplayRasterWidth) << 4;
        }
        else if (addrhi == DISPLAY_HI+1)
        {
           y = (addrlo + DLWORDSPER_SEGMENT) / DisplayRasterWidth;
           x = ((addrlo + DLWORDSPER_SEGMENT) - y * DisplayRasterWidth) << 4;
        }
        else return( NIL );

        if(backward) y -= h;
                
        if((x < MOUSEXR)&&(x + w > MOUSEXL)&&(y < MOUSEYH)&&(y + h > MOUSEYL))
           return( T );
        else 
           return( NIL );
 }
 else {
        base68k=(DLword*)Addr68k_from_LADDR(addrhi << 16 | addrlo);
        if ((ColorDisplayRegion68k<=base68k) &&
                 (base68k <= COLOR_MAX_Address)){
             y =(base68k - ColorDisplayRegion68k) /displaywidth ;
             x = (int)(base68k - ColorDisplayRegion68k) -(y * displaywidth );
                /* printf("cursorin: IN COLOR mx=%d my=%d x=%d y%d w=%d h=%d\n"
                ,*EmMouseX68K,*EmMouseY68K,x,y,w,h); */
        }
        else return( NIL );

       if(backward) y -= h;

       if((x < MOUSEXR)&&((x + (w >> 3)) > MOUSEXL)&&(y < MOUSEYH)&&(y + h > MOUSEYL))
              { /* printf("cursorin T\n"); */ return( T );}
       else
              return( NIL );

 } /* on COLOR screen */
     }
#endif /* COLOR */
