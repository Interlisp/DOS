/* @(#) llcolor.c Version 1.5 (4/21/92). copyright venue & Fuji Xerox  */
static char *id = "@(#) llcolor.c       1.5 4/21/92     (venue & Fuji Xerox)";


/*
*
*
* Copyright (C) 1990 by Fuji Xerox co.,Ltd.. Al rights reserved.
*
*               Author: Mitsunori Matsuda
*               Date  : April 17, 1990
*/



/************************************************************************/
/*                                                                      */
/*      Copyright 1989, 1990 Venue, Fuji Xerox Co., Ltd, Xerox Corp.    */
/*                                                                      */
/*      This file is work-product resulting from the Xerox/Venue        */
/*      Agreement dated 18-August-1989 for support of Medley.           */
/*                                                                      */
/************************************************************************/


#include <stdio.h>

#ifndef NOPIXRECT
#ifndef DOS
#include <sunwindow/window_hs.h>
#include <sunwindow/cms.h>
#include <sunwindow/win_ioctl.h>
#include <pixrect/pixrect_hs.h>
#include <sun/fbio.h>
#include <pixrect/pr_planegroups.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/file.h>
#endif /* DOS */
#endif /* NOPIXRECT */


#include <errno.h>

#include "lispemul.h"
#include "lispmap.h"
#include "lsptypes.h"
#include "address.h"
#include "adr68k.h"
#include "lspglob.h"
#include "emlglob.h"
#include "display.h"
#include "devconf.h"

#include "bb.h"
#include "bitblt.h"
#include "pilotbbt.h" 
#include "dbprint.h"

int MonoOrColor=MONO_SCREEN;
DLword *ColorDisplayRegion68k;
int Dispcolorsize;
int Inited_Color = NIL;
extern struct pixrect *ColorDisplayPixrect;
extern int displaywidth
         , displayheight
         , FrameBufferFd;

/*******************************************************************/
/*      Func name       : cgfour_init_color_display(args)
        Arg(s)          : COLOR BITMAP ADDRESS(LISPPTR)
        Desc            : Assign to SUBR 0210
                          mmap LispPTR to Color Display FB.
        By  Takeshi
*/
/*******************************************************************/
#ifdef COLOR
#ifdef DOS
#include "dspif.h"
extern DspInterface colordsp;
#endif /* DOS */

cgfour_init_color_display(color_bitmapbase) /* SUBR 0210 */
    LispPTR color_bitmapbase;  /* COLOR BITMAP ADDRESS */
    {

#ifdef DOS
      VGA_color_init( colordsp );
#else /* DOS */

    struct pixrect *ColorFb;
    struct pixrect *color_source;
    int mmapstat;

 if(MonoOrColor == COLOR_SCREEN) {
        printf("You can not initialize the color screen from inside color screen. \n");
  }

    ColorDisplayRegion68k = Addr68k_from_LADDR(color_bitmapbase );
    
    Dispcolorsize = ((displaywidth * displayheight + (getpagesize()-1) )
                        & -getpagesize()); /* 8 bit depth */
#ifndef DISPLAYBUFFER
    ColorFb = pr_open("/dev/fb");
#else 
    ColorFb = ColorDisplayPixrect;
#endif /* DISPLAYBUFFER */


#ifndef DISPLAYBUFFER
    color_source = mem_point(displaywidth,displayheight,8,
        ColorDisplayRegion68k);
 
    pr_set_plane_group(ColorFb, PIXPG_8BIT_COLOR);
    pr_rop(ColorFb,0,0,displaywidth,displayheight,PIX_SRC,
        color_source,0,0);

    mmapstat = (int)mmap(ColorDisplayRegion68k ,
                        Dispcolorsize,
                        PROT_READ | PROT_WRITE,
#ifdef OS4
                        MAP_FIXED |
#endif
                        MAP_SHARED,
                        FrameBufferFd, 0x40000 );
    if(mmapstat == -1){
        perror("cgfour_init_color_display: ERROR at mmap system call\n");
        error ("cgfour_init_color_display: ERROR at mmap system call\n You may be able to continue by typing 'q'");
        }
#endif /* DISPLAYBUFFER */


    printf("COLOR-INIT OK BMBASE=0x%x\nNATIVE:= 0x%x)\n",
        color_bitmapbase,ColorDisplayRegion68k);

        Inited_Color = T; /* Color display is active. */

    return(color_bitmapbase);

#endif /* DOS */

    } /* end cgfour_init_color_display */

#else /* COLOR */
cgfour_init_color_display(color_bitmapbase) /* SUBR 0210 */
    LispPTR color_bitmapbase;  /* COLOR BITMAP ADDRESS */
{
        printf( "Color is not supported.\n" );
}
#endif /* COLOR */

/*******************************************************************/
/*      Func name       : cgfour_change_screen_mode(which_screen)
        Arg(s)          : MONO_SCREEN OR COLOR_SCREEN
        Desc            : Assign to SUBR 0211
                          Change screen Mono to Color,vice versa.
        By  Takeshi
*/
/*******************************************************************/
cgfour_change_screen_mode(which_screen)
    LispPTR which_screen;
    { /* subr 0211 */
#ifdef COLOR
#ifdef DOS
    printf("cgfour_change_screen_mode\n");
#else /* DOS */

    struct pixrect *ColorFb;
    extern ScreenLocked;
    extern DLword *EmCursorX68K,*EmCursorY68K;

        int mmapstat;

#ifndef DISPLAYBUFFER
    ColorFb = pr_open("/dev/fb");
#else 
    ColorFb = ColorDisplayPixrect;
#endif /* DISPLAYBUFFER */

    ScreenLocked=T;
    taking_mouse_down();

    switch (which_screen & 0xf) {
        case MONO_SCREEN : { /* resume mono screen */
#ifdef DISPLAYBUFFER
            mmapstat = (int)munmap( ColorDisplayRegion68k, Dispcolorsize );
                if( mmapstat == -1){
                        perror("cg_four_change_screen: ERROR at munmap system call\n");
                        exit( 0 );
                } /* end if(mmapstat) */
            save_color_screen();
#endif /* DISPLAYBUFFER */

            pr_set_plane_group(ColorFb, PIXPG_OVERLAY_ENABLE);
            pr_rop(ColorFb,0,0,
                   ColorFb->pr_width,
                   ColorFb->pr_height, PIX_SET, 0, 0, 0);
            pr_set_plane_group(ColorFb, PIXPG_OVERLAY);
#ifdef DISPLAYBUFFER
            flush_display_buffer();
#endif /* DISPLAYBUFFER */

            MonoOrColor = MONO_SCREEN;
            break;
            }
        case COLOR_SCREEN : {
#ifndef DISPLAYBUFFER
            pr_set_plane_group(ColorFb, PIXPG_OVERLAY_ENABLE);
            pr_rop(ColorFb,0,0,
                   ColorFb->pr_width,
                   ColorFb->pr_height, PIX_CLR, 0, 0, 0);
#endif /* DISPLAYBUFFER */

            pr_set_plane_group(ColorFb, PIXPG_8BIT_COLOR);
#ifdef DISPLAYBUFFER
            restore_color_screen();
            mmapstat = (int)mmap(ColorDisplayRegion68k ,
                                        Dispcolorsize,
                                        PROT_READ | PROT_WRITE,
#ifdef OS4
                                        MAP_FIXED |
#endif
                                        MAP_SHARED,
                                        FrameBufferFd, 0x40000 );
           if(mmapstat == -1){
                perror("cg_four_change_screen: ERROR at mmap system call\n");
                exit(0);
           }
#endif /* DISPLAYBUFFER */

            MonoOrColor = COLOR_SCREEN;
            break;
            }
        default : {
            error("cgfour_change_screen_mode:Unknown mode:");
            }
        };

#ifndef DISPLAYBUFFER
    pr_close(ColorFb);
#endif /* DISPLAYBUFFER */


    taking_mouse_up(*EmCursorX68K,*EmCursorY68K);
    ScreenLocked=NIL;
    return(which_screen);

#endif /* DOS */
#else /* COLOR */
        printf( "Color is not supported.\n" );
#endif /* COLOR */

    }

/*******************************************************************/
/*      Func name       : cgfour_set_colormap(args)
        Arg(s)          : Passed by args
                          index: colormap index(0~255)
                          red,green,blue:(0~255)
        Desc            : Assign to SUBR 0212
                          Set Colormap entry
        By  Takeshi
*/
/*******************************************************************/
unsigned char RED_colormap;
unsigned char GRN_colormap;
unsigned char BLU_colormap;

cgfour_set_colormap(args)
    LispPTR args[];
    {
#ifdef COLOR
#ifdef DOS
    printf("cgfour_set_colormap\n");
#else /* DOS */
    int index;
    struct pixrect *ColorFb;

    index=args[0] & 0xff;

    RED_colormap = (unsigned char) (args[1] & 0xff);
    GRN_colormap = (unsigned char) (args[2] & 0xff);
    BLU_colormap = (unsigned char) (args[3] & 0xff);

    ColorFb = pr_open("/dev/fb");

    if (pr_putcolormap(ColorFb,index,1,
                        &RED_colormap,
                        &GRN_colormap,
                        &BLU_colormap)==-1)
        perror("putcolormap:");
    pr_close(ColorFb);
    return(T);
#endif /* DOS */
#else /* COLOR */
        printf( "Color is not supported.\n" );
#endif /* COLOR */

    }


#ifdef COLOR
#ifdef DISPLAYBUFFER
static struct pixrect *saved_screen;
static int Screen_Saved = T;

save_color_screen()
{
#ifdef DOS
  printf("save_color_screen\n");
#else /* DOS */
        if( !Screen_Saved ) {
                saved_screen = mem_point( displaywidth
                                        , displayheight
                                        , 8
                                        , ColorDisplayRegion68k );
                pr_rop( saved_screen
                        , 0, 0, displaywidth, displayheight
                        , PIX_SRC, ColorDisplayPixrect, 0, 0 );
                Screen_Saved = T;
        } /* end if(!Screen_Saved) */   
#endif /* DOS */
} /* end save_color_screen() */

restore_color_screen()
{
#ifdef DOS
  printf("restore_color_screen\n");
#else /* DOS */
        if( Screen_Saved ) {
                saved_screen = mem_point( displaywidth
                                        , displayheight
                                        , 8
                                        , ColorDisplayRegion68k );
                pr_rop( ColorDisplayPixrect
                        , 0, 0, displaywidth, displayheight
                        , PIX_SRC, saved_screen, 0, 0 );
                Screen_Saved = NIL;
        } /* end if(Screen_Saved) */
#endif /* DOS */
} /* end restore_color_screen() */
#endif /* DISPLAYBUFFER */


static unsigned char red_colormap[256]
                   , green_colormap[256]
                   , blue_colormap[256];
static int Saved_Colormap = NIL;
save_colormap()
{
#ifdef DOS
  printf("save_colormap\n");
#else /* DOS */
        struct pixrect *Color_Fb;

        if( !Saved_Colormap ) {
                Color_Fb = pr_open( "/dev/fb" );
                if( (pr_getcolormap( Color_Fb, 0 , 256
                                        , red_colormap
                                        , green_colormap
                                        , blue_colormap )) == -1 ) 
                        perror("save_color_map:");
                else 
                        Saved_Colormap = T;

                pr_close( Color_Fb );
        } /* end if( !Saved_Colormap ) */
#endif /* DOS */
} /* end save_colormap() */

restore_colormap()
{
#ifdef DOS
  printf("restore_colormap\n");
#else /* DOS */
        struct pixrect *Color_Fb;

        if( Saved_Colormap ) {
                Color_Fb = pr_open( "/dev/fb" );
                if( (pr_putcolormap( Color_Fb, 0 , 256
                                        , red_colormap
                                        , green_colormap
                                        , blue_colormap )) == -1 ) 
                        perror("restore_color_map:");
                pr_close( Color_Fb );
                Saved_Colormap = NIL;
        } /* end if( Saved_Colormap ) */
#endif /* DOS */
} /* end restore_colormap() */
#endif /* COLOR */
