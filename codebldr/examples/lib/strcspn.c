/* STRCSPN.C */
#include <string.h>
#include <stdio.h>

void main()
{
   char string[] = "xyzabc";
   int  pos;

   pos = strcspn( string, "abc" );
   printf( "First a, b or c in %s is at character %d\n", string, pos );
}
