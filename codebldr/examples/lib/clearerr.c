/* CLEARERR.C: This program creates an error on the standard input
 * stream, then clears it so that future reads won't fail.
 */

#include <stdio.h>

void main()
{
   int c;

   /* Create an error by writing to standard input. */
   putc( 'c', stdin );
   if( ferror( stdin ) )
   {
      perror( "Write error" );
      clearerr( stdin );
   }

   /* See if read causes an error. */
   printf( "Will input cause an error? " );
   c = getc( stdin );
   if( ferror( stdin ) )
   {
      perror( "Read error" );
      clearerr( stdin );
   }
}
