/* FPRINTF.C: This program uses fprintf to format various data and
 * print them to the file named FPRINTF.OUT. It then displays
 * FPRINTF.OUT on the screen using the system function to invoke
 * the DOS TYPE command.
 */

#include <stdio.h>
#include <process.h>

FILE *stream;

void main()
{
   int    i = 10;
   double fp = 1.5;
   char   s[] = "this is a string";
   char   c = '\n';

   stream = fopen( "fprintf.out", "w" );
   fprintf( stream, "%s%c", s, c );
   fprintf( stream, "%d\n", i );
   fprintf( stream, "%f\n", fp );
   fclose( stream );
   system( "type fprintf.out" );
}
