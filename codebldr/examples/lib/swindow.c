/* SWINDOW.C: This program illustrates translation between window,
 * view, and physical coordinates. Functions used include:
 *      _setwindow         _getwindowcoord
 *      _getphyscoord      _getviewcoord_wxy
 */

#include <conio.h>
#include <stdlib.h>
#include <graph.h>

enum boolean { FALSE, TRUE };
enum display { MOVE, DRAW, ERASE };

void main()
{
   struct xycoord view, phys;
   struct _wxycoord oldwin, newwin;
   struct videoconfig vc;
   double xunit, yunit, xinc, yinc;
   short  color, key, fintersect = FALSE, fdisplay = TRUE;

   /* Find a valid graphics mode. */
   if( !_setvideomode( _MAXRESMODE ) )
      exit( 1 );
   _getvideoconfig( &vc );

   /* Set a window using real numbers. */
   _setwindow( FALSE, -125.0, -100.0, 125.0, 100.0 );

   /* Calculate the size of one pixel in window coordinates.
    * Then get the current window coordinates and color.
    */
   oldwin = _getwindowcoord( 1, 1 );
   newwin = _getwindowcoord( 2, 2 );
   xunit = xinc = newwin.wx - oldwin.wx;
   yunit = yinc = newwin.wy - oldwin.wy;
   newwin = oldwin = _getcurrentposition_w();
   color = _getcolor();

   while( 1 )
   {
      /* Set flag according to whether current pixel is on, then
       * turn pixel on.
       */
      if( _getpixel_w( oldwin.wx, oldwin.wy ) == color )
         fintersect = TRUE;
      else
         fintersect = FALSE;
      _setcolor( color );
      _setpixel_w( oldwin.wx, oldwin.wy );

      /* Get and test key. */
      key = getch();
      switch( key )
      {
         case 27:                        /* ESC Quit                 */
            _setvideomode( _DEFAULTMODE );
            exit( 0 );
         case 32:                        /* SPACE    Move no color   */
            fdisplay = MOVE;
            continue;
         case 0:                         /* Extended code - get next */
            key = getch();
            switch( key )
            {
               case 72:                  /* UP          -y           */
                  newwin.wy -= yinc;
                  break;
               case 77:                  /* RIGHT    +x              */
                  newwin.wx += xinc;
                  break;
               case 80:                  /* DOWN        +y           */
                  newwin.wy += yinc;
                  break;
               case 75:                  /* LEFT     -x              */
                  newwin.wx -= xinc;
                  break;
               case 82:                  /* INS      Draw white      */
                  fdisplay = DRAW;
                  continue;
               case 83:                  /* DEL      Draw black      */
                  fdisplay = ERASE;
                  continue;
            }
            break;
      }

      /* Translate window coordinates to view, view to physical.
       * Then check physical to make sure we're on screen. Update screen
       * and position if we are. Ignore if not.
       */
      view = _getviewcoord_wxy( &newwin );
      phys = _getphyscoord( view.xcoord, view.ycoord );
      if( (phys.xcoord >= 0) && (phys.xcoord < vc.numxpixels) &&
          (phys.ycoord >= 0) && (phys.ycoord < vc.numypixels) )
      {
         /* If display on, draw to new position, else move to new. */
         if( fdisplay != MOVE )
         {
            if( fdisplay == ERASE )
               _setcolor( 0 );
            _lineto_w( newwin.wx, newwin.wy );
         }
         else
         {
            _setcolor( 0 );
            _moveto_w( newwin.wx, newwin.wy );

            /* If there was no intersect, erase old pixel. */
            if( !fintersect )
               _setpixel_w( oldwin.wx, oldwin.wy );
         }
         oldwin = newwin;
      }
      else
         newwin = oldwin;
   }
}
