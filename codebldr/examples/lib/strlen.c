/* STRLEN.C */
#include <string.h>
#include <stdio.h>
#include <conio.h>
#include <dos.h>

void main()
{
   char buffer[61] = "How long am I?";
   int  len;

   len = strlen( buffer );
   printf( "'%s' is %d characters long\n", buffer, len );
}
