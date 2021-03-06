/* LFIND.C: This program uses lfind to search for the word "hello"
 * in the command-line arguments.
 */

#include <search.h>
#include <string.h>
#include <stdio.h>

int compare( char **arg1, char **arg2 );

void main( int argc, char **argv )
{
   char **result;
   char *key = "hello";

   result = (char **)lfind( (char *)&key, (char *)argv,
                            &argc, sizeof( char * ), compare );
   if( result )
      printf( "%s found\n", *result );
   else
      printf( "hello not found!\n" );
}

int compare(char ** arg1, char **arg2 )
{
   return( strcmpi( *arg1, *arg2 ) );
}
