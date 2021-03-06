/* GVIDCFG.C: This program displays information about the current
 * video configuration.
 */

#include <stdio.h>
#include <graph.h>

void main()
{
   struct videoconfig vc;
   short  c;
   char   b[500];                        /* Buffer for string */

   _setvideomode( _MRES256COLOR );
   _getvideoconfig( &vc );
   _setvideomode( _DEFAULTMODE );

   /* Write all information to a string, then output string. */
   c  = sprintf( b,     "X pixels:     %d\n", vc.numxpixels );
   c += sprintf( b + c, "Y pixels:     %d\n", vc.numypixels );
   c += sprintf( b + c, "Text columns: %d\n", vc.numtextcols );
   c += sprintf( b + c, "Text rows:    %d\n", vc.numtextrows );
   c += sprintf( b + c, "Colors:       %d\n", vc.numcolors );
   c += sprintf( b + c, "Bits/pixel:   %d\n", vc.bitsperpixel );
   c += sprintf( b + c, "Video pages:  %d\n", vc.numvideopages );
   c += sprintf( b + c, "Mode:         %d\n", vc.mode );
   c += sprintf( b + c, "Adapter:      %d\n", vc.adapter );
   c += sprintf( b + c, "Monitor:      %d\n", vc.monitor );
   c += sprintf( b + c, "Memory:       %d\n", vc.memory );
   _outtext( b );
}
