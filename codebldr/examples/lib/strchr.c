/* STRCHR.C: This program illustrates searching for a character with
 * strchr (search forward) or strrchr (search backward).
 */
#include <string.h>
#include <stdio.h>

int  ch = 'r';
char string[] = "The quick brown dog jumps over the lazy fox";
char fmt1[] =   "         1         2         3         4         5";
char fmt2[] =   "12345678901234567890123456789012345678901234567890";

void main()
{
   char *pdest;
   int result;

   printf( "String to be searched: \n\t\t%s\n", string );
   printf( "\t\t%s\n\t\t%s\n\n", fmt1, fmt2 );
   printf( "Search char:\t%c\n", ch );

   /* Search forward */
   pdest = strchr( string, ch );
   result = pdest - string + 1;
   if( pdest != NULL )
      printf( "Result:\tfirst %c found at position %d\n\n", ch, result );
   else
      printf( "Result:\t%c not found\n" );

   /* Search backward */
   pdest = strrchr( string, ch );
   result = pdest - string + 1;
   if( pdest != NULL )
      printf( "Result:\tlast %c found at position %d\n\n", ch, result );
   else
      printf( "Result:\t%c not found\n" );

}
