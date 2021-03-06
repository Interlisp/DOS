/* CSCANF.C: This program prompts for a string and uses cscanf to read
 * in the response. Then cscanf returns the number of items matched,
 * and the sprogram displays that number.
 */

#include <stdio.h>
#include <conio.h>

void main()
{
   int   result, i[3];

   cprintf( "Enter three integers: ");
   result = cscanf( "%i %i %i", &i[0], &i[1], &i[2] );
   cprintf( "\r\nYou entered " );
   while( result-- )
      cprintf( "%i ", i[result] );
   cprintf( "\r\n" );

}
