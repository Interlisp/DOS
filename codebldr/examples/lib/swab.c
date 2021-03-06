/* SWAB.C */
#include <stdlib.h>
#include <stdio.h>

#ifdef BIT16
char from[] = "BADCFEHGJILKNMPORQTSVUXWZY";
#else /* 32 bit swab */
char from[] = "DCBAHGFELKJIPONMTSRQXWVU10ZY54329876";
#endif /* BIT16 */
char to[] =   "..........................";

void main()
{
   printf( "Before:\t%s\n\t%s\n\n", from, to );
#ifdef BIT16
   swab16( from, to, sizeof( from ) );
#else
   swab( from, to, sizeof( from ) );
#endif /* BIT16 */
   printf( "After:\t%s\n\t%s\n\n", from, to );
}
