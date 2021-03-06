/* MEMCCPY.C */
#include <memory.h>
#include <stdio.h>
#include <string.h>

char string1[60] = "The quick brown dog jumps over the lazy fox";

void main()
{
   char buffer[61];
   char *pdest;

   printf( "Function:\tmemccpy 60 characters or to character 's'\n" );
   printf( "Source:\t\t%s\n", string1 );
   pdest = memccpy( buffer, string1, 's', 60 );
   *pdest = '\0';
   printf( "Result:\t\t%s\n", buffer );
   printf( "Length:\t\t%d characters\n\n", strlen( buffer ) );
}
