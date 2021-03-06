#include <conio.h>
#include <stddef.h>
#include <stdlib.h>
#include <malloc.h>
#include <graph.h>

extern int    DisplayWidth, DisplayHeight;

unsigned long *tmpbuffer;

DOSbitbli(source, source_left, source_top, source_width, source_height)
     unsigned long *source;
     int source_left, source_top, source_width, source_height,
{
  unsigned int dest_index = 0;
  unsigned int source_bottom;
  unsigned int source_right;
  unsigned int dest_row, source_row, dest_col, source_col;
  unsigned int rowlimit;
  unsigned int pixel;

  source_right = (source_left + source_width + 31)/32;
  source_bottom = source_top + source_height;

  /* Adjust to be on word boundrys */
  source_left /= 32;

  tmpbuffer[dest_index++] = (source_heigoht << 16) + ((source_right - source_left) * 32);

  /* Load the bitimage into the body of the buffer */
                       
  for (source_row = source_top;
       source_row < source_bottom;
       source_row++) {
    for (pixel=0; pixel < 4; pixel++)
      for (source_col = ((source_row*DisplayWidth)/32) + source_left, rowlimit = source_col + source_right - source_left;
	   source_col < rowlimit;
	   source_col++) {
	tmpbuffer[dest_index++] = ~(((0xffff0000 & (((0xff00ff00 & source[source_col]) >> 8) +
						   ((0x00ff00ff & source[source_col]) << 8))) >> 16) +
				   ((0x0000ffff & (((0xff00ff00 & source[source_col]) >> 8) +
						   ((0x00ff00ff & source[source_col]) << 8))) << 16));
      }
  }
  _putimage_w((source_left * 32), source_top, (char *)tmpbuffer, _GPSET );
}

#ifdef NEVER
DOSdspinit()
{
  struct videoconfig vc;

  /* Init the screen to max resolution. */
  if( !_setvideomode( _MAXRESMODE ) )
    exit( 1 );

  _getvideoconfig( &vc );

  DisplayWidth = vc.numxpixels;
  DisplayHeight = vc.numypixels;

  /* Measure the image to be drawn and allocate memory for it. */
  tmpbuffer = halloc((size_t)(_imagesize_w(0, 0, DisplayWidth, DisplayHeight)),
		     sizeof( char ));
  if ( tmpbuffer == (unsigned long *)NULL )
    exit( 1 );

}
void exitfree( buffer )
     char *buffer;
{
    hfree( buffer );
    exit( !_setvideomode( _DEFAULTMODE ) );
}

#endif /* NEVER */

