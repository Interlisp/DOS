/* BKEYBRD.C: This program prints a message on the screen until the
 * right SHIFT key is pressed.
 */

#include <bios.h>
#include <stdio.h>

void main()
{
   while( !(_bios_keybrd( _KEYBRD_SHIFTSTATUS ) & 0001) )
      printf( "Use the right SHIFT key to stop this message\n" );
   printf( "Right SHIFT key pressed\n" );
}
