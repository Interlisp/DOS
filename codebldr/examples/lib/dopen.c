/* DOPEN.C: This program uses DOS I/O functions to open and close a file. */

#include <fcntl.h>
#include <stdio.h>
#include <dos.h>

void main()
{
   int fh;

   /* Open file with _dos_open function */
   if( _dos_open( "data1", O_RDONLY, &fh ) != 0 )
      perror( "Open failed on input file\n" );
   else
      printf( "Open succeeded on input file\n" );

   /* Close file with _dos_close function */
   if( _dos_close( fh ) != 0 )
      perror( "Close failed\n" );
   else
      printf( "File successfully closed\n" );
}
