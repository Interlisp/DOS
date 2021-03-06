/* ASSERT.C: In this program, the analyze_string function uses the
 * assert function to test several conditions related to string and
 * length. If any of the conditions fails, the program prints a
 * message indicating what caused the failure.
 */

#include <stdio.h>
#include <assert.h>
#include <string.h>

void analyze_string( char *string );   /* Prototype */

void main()
{
   char  test1[] = "abc", *test2 = NULL, test3[] = "";

   printf ( "Analyzing string '%s'\n", test1 );
   analyze_string( test1 );
   printf ( "Analyzing string '%s'\n", test2 );
   analyze_string( test2 );
   printf ( "Analyzing string '%s'\n", test3 );
   analyze_string( test3 );
}

/* Tests a string to see if it is NULL, empty, or longer than 0 characters */
void analyze_string( char * string )
{
   assert( string != NULL );        /* Cannot be NULL */
   assert( *string != '\0' );       /* Cannot be empty */
   assert( strlen( string ) > 2 );  /* Length must be greater than 2 */
}
