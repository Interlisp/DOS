/* SEARCHEN.C: This program searches for a file in a directory
 * specified by an environment variable.
 */

#include <stdlib.h>
#include <stdio.h>
 
void main()
{
   char pathbuffer[_MAX_PATH];
   char searchfile[] = "CL.EXE";
   char envvar[] = "PATH";

   /* Search for file in PATH environment variable: */
   _searchenv( searchfile, envvar, pathbuffer );
   if( *pathbuffer != '\0' )
      printf( "Path for %s: %s\n", searchfile, pathbuffer );
   else
      printf( "%s not found\n", searchfile );
}
