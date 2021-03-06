/* QSORT.C: This program reads the command-line parameters and
 * uses qsort to sort them. It then displays the sorted arguments.
 */

#include <search.h>
#include <string.h>
#include <stdio.h>

int compare( char **arg1, char **arg2 );  /* Prototype */

void main( int argc, char **argv )
{
   int i;

   /* Eliminate argv[0] from sort: */
   argv++;
   argc--;

   /* Sort remaining args using Quicksort algorithm: */
   qsort( (void *)argv, (size_t)argc, sizeof( char * ), compare );

   /* Output sorted list: */
   for( i = 0; i < argc; ++i )
      printf( "%s ", argv[i] );
   printf( "\n" );
}

int compare( char **arg1, char **arg2 )
{
   /* Compare all of both strings: */
   return strcmpi( *arg1, *arg2 );
}
