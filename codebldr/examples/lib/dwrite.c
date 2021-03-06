/* DWRITE.C: This program uses DOS I/O functions to write to a file. */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>

void main()
{
   char out_buffer[] = "Hello";
   int  fh;
   unsigned n_written;

   /* Open file with _dos_creat function */
   if( _dos_creat( "data", _A_NORMAL, &fh ) == 0 )
   {
      /* Write data with _dos_write function */
      _dos_write( fh, out_buffer, 5, &n_written );
      printf( "Number of characters written: %d\n", n_written );

      _dos_close( fh );
      printf( "Contents of file are:\n" );
      system( "type data" );
   }
}
