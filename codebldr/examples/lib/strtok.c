/* STRTOK.C: In this program, a loop uses strtok to print all the tokens
 * (separated by commas or blanks) in the string named "string".
 */

#include <string.h>
#include <stdio.h>

char string[] = "A string\tof ,,tokens\nand some  more tokens";
char seps[]   = " ,\t\n";
char *token;

void main()
{

   printf( "%s\n\nTokens:\n", string );

   /* Establish string and get the first token: */
   token = strtok( string, seps );
   while( token != NULL )
   {
      /* While there are tokens in "string" */
      printf( " %s\n", token );
      /* Get next token: */
      token = strtok( NULL, seps );
   }
}
