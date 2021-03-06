/* PUTC.C: This program uses putc to write buffer to a stream.
 * If an error occurs, the program will stop before writing the
 * entire buffer.
 */

#include <stdio.h>

void main()
{
   FILE *stream;
   char *p, buffer[] = "This is the line of output\n";
   int  ch;

   /* Make standard out the stream and write to it. */
   stream = stdout;
   for( p = buffer; (ch != EOF) && (*p != '\0'); p++ )
      ch = putc( *p, stream );
}
