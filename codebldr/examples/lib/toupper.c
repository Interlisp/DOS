/* TOUPPER.C: This program uses toupper and tolower to analyze all
 * characters between 0x0 and 0x7F. It also applies _toupper and _tolower
 * to any code in this range for which these functions make sense.
 */

#include <conio.h>
#include <ctype.h>
#include <string.h>

char msg[] = "Some of THESE letters are Capitals\r\n";
char *p;

void main()
{
   cputs( msg );

   /* Reverse case of message. */
   for( p = msg; p < msg + strlen( msg ); p++ )
   {
      if( islower( *p ) )
         putch( _toupper( *p ) );
      else if( isupper( *p ) )
         putch( _tolower( *p ) );
      else
         putch( *p );
   }
}
