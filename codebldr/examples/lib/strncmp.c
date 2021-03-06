/* STRNCMP.C */
#include <string.h>
#include <stdio.h>

char string1[] = "The quick brown dog jumps over the lazy fox";
char string2[] = "The QUICK brown fox jumps over the lazy dog";

void main()
{
   char tmp[20];
   int result;

   printf( "Compare strings:\n\t\t%s\n\t\t%s\n\n", string1, string2 );

   printf( "Function:\tstrncmp (first 10 characters only)\n" );
   result = strncmp( string1, string2 , 10 );
   if( result > 0 )
      strcpy( tmp, "greater than" );
   else if( result < 0 )
      strcpy( tmp, "less than" );
   else
      strcpy( tmp, "equal to" );
   printf( "Result:\t\tString 1 is %s string 2\n\n", tmp );

   printf( "Function:\tstrnicmp (first 10 characters only)\n" );
   result = strnicmp( string1, string2, 10 );
   if( result > 0 )
      strcpy( tmp, "greater than" );
   else if( result < 0 )
      strcpy( tmp, "less than" );
   else
      strcpy( tmp, "equal to" );
   printf( "Result:\t\tString 1 is %s string 2\n\n", tmp );

}
