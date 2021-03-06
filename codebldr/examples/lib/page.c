/* PAGE.C illustrates video page functions including:
 *      _getactivepage  _getvisualpage  _setactivepage  _setvisualpage
 */

#include <conio.h>
#include <graph.h>
#include <stdlib.h>

void main()
{
   short  oldvpage, oldapage, page, row, col, line;
   struct videoconfig vc;
   char   buf[80];

   _getvideoconfig( &vc );
   if( vc.numvideopages < 4 )
       exit( 1 );              /* Fail for OS/2 or monochrome. */
   oldapage  = _getactivepage();
   oldvpage  = _getvisualpage();
   _displaycursor( _GCURSOROFF );

   /* Draw arrows in different place on each page. */
   for( page = 1; page < 4; page++ )
   {
      _setactivepage( page );
      _settextposition( 12, 16 * page );
      _outtext( ">>>>>>>>" );
   }

   while( !kbhit() )
      /* Cycle through pages 1 to 3 to show moving image. */
      for( page = 1; page < 4; page++ )
          _setvisualpage( page );
   getch();

   /* Restore original page (normally 0) to restore screen. */
   _setactivepage( oldapage );
   _setvisualpage( oldvpage );
   _displaycursor( _GCURSORON );
}
