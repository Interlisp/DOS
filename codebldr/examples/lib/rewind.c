/* REWIND.C: This program first opens a file named REWIND.OUT for input and
 * output and writes two integers to the file. Next, it uses rewind to
 * reposition the file pointer to the beginning of the file and reads
 * the data back in.
 */

#include <stdio.h>

void main()
{
   FILE *stream;
   int data1, data2;

   data1 = 1;
   data2 = -37;

   if( (stream = fopen( "rewind.out", "w+" )) != NULL )
   {
      fprintf( stream, "%d %d", data1, data2 );
      printf( "The values written are: %d and %d\n", data1, data2 );
      rewind( stream );
      fscanf( stream, "%d %d", &data1, &data2 );
      printf( "The values read are: %d and %d\n", data1, data2 );
      fclose( stream );
   }
}
