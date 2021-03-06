/* RENAMER.C: This program attempts to rename a file named RENAMER.OBJ to
 * RENAMER.JBO. For this operation to succeed, a file named RENAMER.OBJ
 * must exist and a file named RENAMER.JBO must not exist.
 */

#include <stdio.h>

void main()
{
   int  result;
   char old[] = "RENAMER.OBJ", new[] = "RENAMER.JBO";

   /* Attempt to rename file: */
   result = rename( old, new );
   if( result != 0 )
      printf( "Could not rename '%s'\n", old );
   else
      printf( "File '%s' renamed to '%s'\n", old, new );
}
