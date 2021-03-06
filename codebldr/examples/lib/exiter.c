/* EXITER.C: This program prompts the user for a yes or no and returns
 * a DOS error code of 1 if the user answers Y or y; otherwise it
 * returns 0. The error code could be tested in a batch file.
 */

#include <conio.h>
#include <stdlib.h>

void main()
{
   char  ch;

   cputs( "Yes or no? " );
   ch = getch();
   cputs( "\r\n" );
   if( toupper( ch ) == 'Y' )
      exit( 1 );
   else
      exit( 0 );
}
