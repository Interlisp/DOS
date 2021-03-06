/* SCRTXWIN.C: This program displays text in text windows and then
 * scrolls, inserts, and deletes lines.
 */

#include <stdio.h>
#include <conio.h>
#include <graph.h>

void deleteline( void );
void insertline( void );
void status( char *msg );

void main()
{
   short row;
   char  buf[40];

   /* Set up screen for scrolling, and put text window around scroll area. */
   _settextrows( 25 );
   _clearscreen( _GCLEARSCREEN );
   for( row = 1; row <= 25; row++ )
   {
      _settextposition( row, 1 );
      sprintf( buf, "Line %c             %2d", row + 'A' - 1, row );
      _outtext( buf );
   }
   getch();
   _settextwindow( 1, 1, 25, 10 );

   /* Delete some lines. */
   _settextposition( 11, 1 );
   for( row = 12; row < 20; row++ )
      deleteline();
   status( "Deleted 8 lines" );

   /* Insert some lines. */
   _settextposition( 5, 1 );
   for( row = 1; row < 6; row++ )
      insertline();
   status( "Inserted 5 lines" );

   /* Scroll up and down. */
   _scrolltextwindow( -7 );
   status( "Scrolled down 7 lines" );
   _scrolltextwindow( 5 );
   status( "Scrolled up 5 lines" );
   _setvideomode( _DEFAULTMODE );
}

/* Delete lines by scrolling them off the top of the current text window.
 * Save and restore original window.
 */
void deleteline()
{
   short left, top, right, bottom;
   struct rccoord rc;

   _gettextwindow( &top, &left, &bottom, &right );
   rc = _gettextposition();
   _settextwindow( rc.row, left, bottom, right );
   _scrolltextwindow( _GSCROLLUP );
   _settextwindow( top, left, bottom, right );
   _settextposition( rc.row, rc.col );
}

/* Insert some lines by scrolling in blank lines from the top of the
 * current text window. Save and restore original window.
 */
void insertline()
{
   short left, top, right, bottom;
   struct rccoord rc;

   _gettextwindow( &top, &left, &bottom, &right );
   rc = _gettextposition();
   _settextwindow( rc.row, left, bottom, right );
   _scrolltextwindow( _GSCROLLDOWN );
   _settextwindow( top, left, bottom, right );
   _settextposition( rc.row, rc.col );
}

/* Display and clear status in its own window. */
void status( char *msg )
{
   short left, top, right, bottom;
   struct rccoord rc;

   _gettextwindow( &top, &left, &bottom, &right );
   _settextwindow( 1, 50, 2, 80 );
   _outtext( msg );
   getch();
   _clearscreen( _GWINDOW );
   _settextwindow( top, left, bottom, right );
}
