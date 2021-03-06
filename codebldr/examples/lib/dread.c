/* DREAD.C: This program uses the DOS I/O operations to read the contents
 * of a file.
 */

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <dos.h>

void main()
{
   int fh;
   char buffer[50];
   unsigned number_read;

   /* Open file with _dos_open function */
   if( _dos_open( "dread.c", O_RDONLY, &fh ) != 0 )
      perror( "Open failed on input file\n" );
   else
      printf( "Open succeeded on input file\n" );

   /* Read data with _dos_read function */
   _dos_read( fh, buffer, 50, &number_read );
   printf( "First 40 characters are: %.40s\n\n", buffer );

   /* Close file with _dos_close function */
   _dos_close( fh );
}
