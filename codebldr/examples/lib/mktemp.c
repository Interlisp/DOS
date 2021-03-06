/* MKTEMP.C: The program uses mktemp to create five unique file names.
 * It opens each file name to ensure that the next name is unique.
 */

#include <io.h>
#include <string.h>
#include <stdio.h>

char *template = "fnXXXXXX";
char *result;
char names[5][9];

void main()
{
   int i;
   FILE *fp;

   for( i = 0; i < 5; i++ )
   {
      strcpy( names[i], template );

      /* Attempt to find a unique file name: */
      result = mktemp( names[i] );
      if( result == NULL )
         printf( "Problem creating the template" );
      else
      {
         if( (fp = fopen( result, "w" )) != NULL )
             printf( "Unique file name is %s\n", result );
         else
             printf( "Cannot open %s\n", result );
         fclose( fp );
      }
   }
}
