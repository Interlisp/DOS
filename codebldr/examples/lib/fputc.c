/* FPUTC.C: This program uses fputc and fputchar to send a character
 * array to stdout.
 */

#include <stdio.h>

void main()
{
   char strptr1[] = "This is a test of fputc!!\n";
   char strptr2[] = "This is a test of fputchar!!\n";
   char *p;

   /* Print line to stream using fputc. */
   p = strptr1;
   while( (*p != '\0') && fputc( *(p++), stdout ) != EOF )
      ;

   /* Print line to stream using fputchar. */
   p = strptr2;
   while( (*p != '\0') && fputchar( *(p++) ) != EOF )
      ;
}
