/* BDOS.C: This example calls DOS function 0x9 (display string)
 * to display a $-terminated string.
 */

#include <dos.h>

/* Function 0x09 assumes that DS will contain segment of the string.
 * This will be true for all memory models if the string is declared near.
 */
char _near str[] = "Hello world!\r\n$";

void main()
{
   /* Offset of string must be in DX, segment in DS. AL is not needed,
    * so 0 is used.
    */
   bdos( 0x09, (int)str, 0 );
}
