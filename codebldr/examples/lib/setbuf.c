/* SETBUF.C: This program first opens files named DATA1 and DATA2.
 * Then it uses setbuf to give DATA1 a user-assigned buffer
 * and to change DATA2 so that it has no buffer.
 */

#include <stdio.h>

void main()
{
   char buf[BUFSIZ];
   FILE *stream1, *stream2;

   if( ((stream1 = fopen( "data1", "a" )) != NULL) &&
       ((stream2 = fopen( "data2", "w" )) != NULL) )
   {
      /* "stream1" uses user-assigned buffer: */
      setbuf( stream1, buf );
      printf( "stream1 set to user-defined buffer at: %Fp\n", buf );

      /* "stream2" is unbuffered              */
      setbuf( stream2, NULL );
      printf( "stream2 buffering disabled\n" );
      fcloseall();
   }
}
