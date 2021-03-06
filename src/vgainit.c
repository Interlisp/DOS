#include <stdio.h>
#include <graph.h>
#include "lispemul.h"
#include "dspif.h"

extern void Dosbbt1();
extern void Dosbbt2();
extern void GenericPanic();

void VGA_setmax( dsp )
     DspInterface dsp;
{
  struct videoconfig vc;

  if( !_setvideomode( _MAXRESMODE ) ) {
    fprintf ( stderr, "Can't set graphics mode.\n" );
    exit( 1 );
  }
  _getvideoconfig( &vc );
  dsp->displaywidth = vc.numxpixels;
  dsp->displayheight = vc.numypixels;
  dsp->bitsperpixel = vc.bitsperpixel;
  dsp->colors = vc.numcolors;
/*  printf("width = %d, height = %d, bpp = %d, colors = %d\n", vc.numxpixels,
				vc.numypixels, vc.bitsperpixel, vc.numcolors); fflush(stdout); getch();
*/
}

void VGA_exit( dsp )
  DspInterface dsp;
{
  _setvideomode( _DEFAULTMODE );
}

unsigned long VGA_not_color( dsp )
     DspInterface dsp;
{
  printf( "Colormode not set!\n");
  fflush(stdout);
  return(0);
}

unsigned long VGA_colornum( dsp )
     DspInterface dsp;
{ /* Return the number of available colors */
  return(1 << dsp->bitsperpixel);
}

unsigned long VGA_possiblecolors( dsp )
     DspInterface dsp;
{
  return(dsp->colors);
}

void VGA_mono_drawline( dsp, startX, startY, width, height, function, color,
		  thickness, butt, clipX, clipY, clipWidth, clipHeight, dashing)
     DspInterface dsp;
     unsigned long startX, startY, width, height;
     int function;
     unsigned long color, thickness;
     int butt;
     unsigned long clipX, clipY, clipWidth, clipHeight;
     LispPTR *dashing;
  
{
  _moveto_w( startX, startY );
  _lineto_w( width, height );  
}

void VGA_color_drawline ( dsp, startX, startY, width, height, function, color,
		    thickness, butt, clipX, clipY, clipWidth, clipHeight, dashing)
     DspInterface dsp;
     unsigned long startX, startY, width, height;
     int function;
     unsigned long color, thickness;
     int butt;
     unsigned long clipX, clipY, clipWidth, clipHeight;
     LispPTR *dashing;

{
  
}

void VGA_cleardisplay ( dsp )
     DspInterface dsp;
{
  _clearscreen(_GCLEARSCREEN);
}

VGA_init( dsp, lispbitmap, width_hint, height_hint, depth_hint )
     DspInterface dsp;
     char *lispbitmap;
     int width_hint, height_hint, depth_hint;
{
  struct videoconfig vc;

  dsp->enter_display = &VGA_setmax;
  dsp->exit_display = &VGA_exit;

  dsp->before_raid = &VGA_exit;
  dsp->after_raid = &VGA_setmax;
    
  dsp->drawline = &VGA_mono_drawline;

  dsp->cleardisplay = &VGA_cleardisplay;

  dsp->get_color_map_entry = &VGA_not_color;
  dsp->set_color_map_entry = &VGA_not_color;
  dsp->available_colors = &VGA_not_color;
  dsp->possible_colors = &VGA_not_color;

  dsp->medley_to_native_bm = &GenericPanic;
  dsp->native_to_medley_bm = &GenericPanic;

  dsp->bitblt_to_screen = &Dosbbt2;
  dsp->bitblt_from_screen = &GenericPanic;
  dsp->scroll_region = &GenericPanic;

  dsp->DisplayStartAddr = 0xa0000;
  dsp->BytesPerLine = 80;
}

VGA_color_init( dsp )
     DspInterface dsp;
{
  struct videoconfig vc;

  dsp->enter_display = &VGA_setmax;
  dsp->exit_display = &VGA_exit;

  dsp->before_raid = &VGA_exit;
  dsp->after_raid = &VGA_setmax;
    
  dsp->drawline = &GenericPanic;

  dsp->cleardisplay = &VGA_cleardisplay;

  dsp->get_color_map_entry = &VGA_not_color;
  dsp->set_color_map_entry = &VGA_not_color;
  dsp->available_colors = &VGA_not_color;
  dsp->possible_colors = &VGA_not_color;

  dsp->medley_to_native_bm = &GenericPanic;
  dsp->native_to_medley_bm = &GenericPanic;

/*   dsp->bitblt_to_screen = &DosbbtC1; */
  dsp->bitblt_from_screen = &GenericPanic;
  dsp->scroll_region = &GenericPanic;

  VGA_setmax( dsp );
}
