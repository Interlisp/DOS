/* BSEARCH.C: This program reads the command-line arguments, sorting them
 * with qsort, and then uses bsearch to find the word "cat."
 */

#include <search.h>
#include <string.h>
#include <stdio.h>

int compare( char **arg1, char **arg2 );  /* Declare a function for compare */

void main( int argc, char **argv )
{

   char **result;
   char *key = "cat";
   int i;

   /* Sort using Quicksort algorithm: */
   qsort( (char *)argv, argc, sizeof( char * ), compare );

   for( i = 0; i < argc; ++i )        /* Output sorted list */
      printf( "%s ", argv[i] );

   /*  Find the word "cat" using a binary search algorithm: */
   result = (char **)bsearch( (char *) &key, (char *)argv, argc,
                              sizeof( char * ), compare );
   if( result )
      printf( "\n%s found at %Fp\n", *result, result );
   else
      printf( "\nCat not found!\n" );
}

int compare( char **arg1, char **arg2 )
{
   /* Compare all of both strings: */
   return strcmpi( *arg1, *arg2 );
}
