/* SCANF.C: This program receives formatted input using scanf. */
#include <stdio.h>

void main()
{
   int   i;
   float fp;
   char  c, s[81];
   int   result;

   printf( "Enter an integer, a floating-point number, "
           "a character and a string:\n" );
   result = scanf( "%d %f %c %s", &i, &fp, &c, s );

   printf( "\nThe number of fields input is %d\n", result );
   printf( "The contents are: %d %f %c %s\n", i, fp, c, s );
}
