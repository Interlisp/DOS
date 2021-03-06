/* PRINTF.C illustrates output formatting with printf. */

#include <stdio.h>

void main()
{
   char   ch = 'h', *string = "computer";
   int    count = -9234;
   double fp = 251.7366;

   /* Display integers. */
   printf( "Integer formats:\n"
           "\tDecimal: %d  Justified: %.6d  Unsigned: %u\n",
            count, count, count, count );

   printf( "Decimal %d as:\n\tHex: %Xh  C hex: 0x%x  Octal: %o\n",
            count, count, count, count );

   /* Display in different radixes. */
   printf( "Digits 10 equal:\n\tHex: %i  Octal: %i  Decimal: %i\n",
            0x10, 010, 10 );

   /* Display characters. */
   printf( "Characters in field:\n%10c   %5c\n", ch, ch );

   /* Display strings. */
   printf( "Strings in field:\n%25s\n%25.4s\n", string, string );

   /* Display real numbers. */
   printf( "Real numbers:\n\t%f    %.2f    %e    %E\n", fp, fp, fp, fp );

   /* Display pointers. */
   printf( "Address as:\n\tDefault: %p  Near: %Np  Far: %Fp\n",
            &count, (int _near *)&count, (int _far *)&count );

   /* Count characters printed. */
   printf( "Display to here:\n" );
   printf( "1234567890123456%n78901234567890\n", &count );
   printf( "\tNumber displayed: %d\n\n", count );

}
