/* Super VGA routines for DOS. */
#include <conio.h>

#define ET3000	0x20
#define ET4000	0x21

/* G e t _ c h i p s e t returns the type of chipset we are dealing with.*/
get_chipset()
{
  if (vga_tseng_p())
    if(vga_tseng4k_p()) {
      return( ET4000 );
      } else {
	return( ET3000 );
      }
}

test_bits_rw(port, bits)
     int port, bits;
{
  int new_value,old_value, value;
  old_value = inp(port);	/* Get old value */

  outp(port, (old_value & (~bits))); /* Test inverted bits */
  value = (inp(port) & bits);

  outp(port, old_value | bits);	/* Test straight bits */
  new_value = (inp(port) & bits);

  outp(port, old_value);	/* Restore old value */
  return((value == 0) & (new_value > 0));
}

test_bits2(port, reg, bits)
     int port, reg, bits;
{
  outp(port, reg);
  test_bits_rw(port++, bits);
}

inp_with_index(port, index)
     int port, index;
{
  outp(port, index);
  return (inp(port++));
}

vgaIObase()
{
  (inb(0x3cc) & 0x01) ? 0x3d0 : 0x3b0;
}

/********************************************************************/
/* v g a _ t s e n g _ p if we can twiddle the high bits in the ATC */
/* Misc. register we have a Tseng chip.                             */
/********************************************************************/
vga_tseng_p()
{
  unsigned char new_value,old_value, value;

  test_bits_rw(0x3cd, 0x3f);
}

/********************************************************************/
/* v g a _ t s e n g 4 k _ p if we can twiddle the high bits in the */
/* CRTC. register we have a Tseng chip.                             */
/********************************************************************/
vga_tseng4k_p()
{
  test_bits2((inp (0x3cc) & 0x01) ? 0x3d4 : 0x3b4, 0x33, 0xf);
}


main()
{

  switch (get_chipset()) {
  case ET3000 :
    printf("Et 3000\n");
    break;
  case ET4000 :
    printf("Et 4000\n");
    break;
    default :
      printf ("Foot\n");
  }
}
