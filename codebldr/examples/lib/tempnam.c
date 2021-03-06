/* TEMPNAM.C: This program uses tmpnam to create a unique file name in
 * the current working directory, then uses tempnam to create a unique
 * file name with a prefix of stq.
 */

#include <stdio.h>

void main()
{
   char *name1, *name2;

   /* Create a temporary file name for the current working directory: */
   if( ( name1 = tmpnam( NULL ) ) != NULL )
      printf( "%s is safe to use as a temporary file.\n", name1 );
   else
      printf( "Cannot create a unique file name\n" );

   /* Create a temporary file name in temporary directory with the
    * prefix "stq". The actual destination directory may vary depending
    * on the state of the TMP environment variable and the global variable
    * P_tmpdir.
    */
   if( ( name2 = tempnam( "c:\\tmp", "stq" ) ) != NULL )
      printf( "%s is safe to use as a temporary file.\n", name2 );
   else
      printf( "Cannot create a unique file name\n" );
}
