/* STRDUP.C */
#include <string.h>
#include <stdio.h>
#include <conio.h>
#include <dos.h>

void main()
{
   char buffer[] = "This is the buffer text";
   char *newstring;

   printf( "Original: %s\n", buffer );
   newstring = strdup( buffer );
   printf( "Copy:     %s\n", newstring );
}
