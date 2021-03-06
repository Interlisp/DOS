/* CGETS.C: This program creates a buffer and initializes the first byte
 * to the size of the buffer - 2. Next, the program accepts an input string
 * using cgets and displays the size and text of that string.
 */

#include <conio.h>
#include <stdio.h>

void main()
{
   char buffer[82] = { 80 };  /* Maximum characters in first byte */
   char *result;

   printf( "Input line of text, followed by carrage return:\n");
   result = cgets( buffer );  /* Input a line of text */
   printf( "\nLine length = %d\nText = %s\n", buffer[1], result );
}
