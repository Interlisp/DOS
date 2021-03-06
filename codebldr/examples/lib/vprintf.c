/* VPRINTF.C shows how to use vprintf functions to write new versions
 * of printf. The vsprintf function is used in the example.
 */

#include <stdio.h>
#include <graph.h>
#include <string.h>
#include <stdarg.h>
#include <malloc.h>

int wprintf( short row, short col, short clr, long bclr, char *fmt, ... );

void main()
{
   short fgd = 0;
   long  bgd = 0L;

   _clearscreen( _GCLEARSCREEN );
   _outtext( "Color text example:\n\n" );

   /* Loop through 8 background colors. */
   for( bgd = 0L; bgd < 8; bgd++ )
   {
      wprintf( (int)bgd + 3, 1, 7, bgd, "Back: %d Fore:", bgd );

      /* Loop through 16 foreground colors. */
      for( fgd = 0; fgd < 16; fgd++ )
         wprintf( -1, -1, fgd, -1L, " %2d ", fgd );
   }
}

/* Full-screen window version of printf that takes row, column, textcolor,
 * and background color as its first arguments, followed by normal printf
 * format strings (except that \t is not handled). You can specify -1 for
 * any of the first arguments to use the current value. The function returns
 * the number of characters printed, or a negative number for errors.
 */
int wprintf( short row, short col, short clr, long bclr, char *fmt, ... )
{
   struct  rccoord tmppos;
   short   ret, size;
   va_list marker;
   char    *buffer;

   /* It's probably safe to use a buffer length of 512 bytes or five times
    * the length of the format string.
    */
   size = strlen( fmt );
   size = (size > 512) ? 512 : size * 5;
   if( (buffer = (char *)malloc( size )) == NULL )
      return -1;

   /* Set text position. */
   tmppos = _gettextposition();
   if( row < 1 )
      row = tmppos.row;
   if( col < 1 )
      col = tmppos.col;
   _settextposition( row, col );

   /* Set foreground and background colors. */
   if( clr >= 0 )
      _settextcolor( clr );
   if( bclr >= 0 )
      _setbkcolor( bclr );

   /* Write text to a string and output the string. */
   va_start( marker, fmt );
   ret = vsprintf( buffer, fmt, marker );
   va_end( marker );
   _outtext( buffer );
   free( buffer );
   return ret;
}
