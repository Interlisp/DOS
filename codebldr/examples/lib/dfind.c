/* DFIND.C: This program finds and prints all files in the current
 * directory with the .c extension.
 */

#include <stdlib.h>
#include <stdio.h>
#include <dos.h>

void main()
{
   struct find_t  c_file;

   /* find first .c file in current directory */
   if( _dos_findfirst( "d:\\src\\*.c", _A_NORMAL, &c_file ) )
   {
      printf( "No .c files\n" );
      exit( 1 );
   }

   printf( "Listing of .c files\n\n" );
   printf( "File: %s is %ld bytes\n", c_file.name, c_file.size );

   /* find the rest of the .c files */
   while( _dos_findnext( &c_file ) == 0 )
      printf( "File: %s is %ld bytes\n", c_file.name, c_file.size );
}
