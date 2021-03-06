/* ISFAM.C: This program tests all characters between 0x0 and 0x7F,
 * then displays each character with abbreviations for the character-type
 * codes that apply.
 */

#include <stdio.h>
#include <ctype.h>

void main()
{
   int ch;
   for( ch = 0; ch <= 0x7f; ch++ )
   {
      printf( "%.2x ", ch );
      printf( " %c", isprint( ch )  ? ch   : '\0' );
      printf( "%4s", isalnum( ch )  ? "AN" : "" );
      printf( "%3s", isalpha( ch )  ? "A"  : "" );
      printf( "%3s", isascii( ch )  ? "AS" : "" );
      printf( "%3s", iscntrl( ch )  ? "C"  : "" );
      printf( "%3s", isdigit( ch )  ? "D"  : "" );
      printf( "%3s", isgraph( ch )  ? "G"  : "" );
      printf( "%3s", islower( ch )  ? "L"  : "" );
      printf( "%3s", ispunct( ch )  ? "PU" : "" );
      printf( "%3s", isspace( ch )  ? "S"  : "" );
      printf( "%3s", isprint( ch )  ? "PR" : "" );
      printf( "%3s", isupper( ch )  ? "U"  : "" );
      printf( "%3s", isxdigit( ch ) ? "X"  : "" );
      printf( "\n" );
   }
}
