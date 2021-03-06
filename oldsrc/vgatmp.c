#include <conio.h>
#include <stddef.h>
#include <stdlib.h>
#include <malloc.h>
#include <graph.h>
#include <dos.h>

unsigned char *tmpbuffer;
unsigned long DisplayWidth;
unsigned long DisplayWidth32;
unsigned long DisplayWidth8;
unsigned long DisplayHeight;

TS_outp(index, value)
     int index, value;
{
  outp( 0x3c4, index);
  outp( 0x3c5, value);
}

CRTC_outp(index, value)
     int index, value;
{
  outp( 0x3d4, index);
  outp( 0x3d5, value);
}

GDC_outp(index, value)
     int index, value;
{
  outp( 0x3ce, index);
  outp( 0x3cf, value);
}

ATC_outp(index, value)
     int index, value;
{
  inp(vgaIObase() + 0xa);
  outp( 0x3c0, index);
  outp( 0x3c0, value);
}

MISC_outp(dummy, value)
     int dummy, value;
{
  outp( 0x3c2, value );		/* Misc out */
}

clockselect(nr)
     int nr;
{
  unsigned char tmp;
  tmp = inp(0x3cc);
  outp(0x3c2, (tmp & 0xf) | (( nr << 2) & 0x0c));
  outp(vgaIObase() + 4, 0x34 | ((nr & 0x4) << 7));
}

vesacall(class, func)
     int class, func;
{
  union REGS inregs, outregs;

  inregs.h.ah = 0x4f;
  inregs.h.al = class;
  inregs.x.bx = func;
  int86( 0x10, &inregs, &outregs );
  if (outregs.h.al != 0x4f)
    printf("VESA function %d %d NOT supported\n", class, func);
  if (outregs.h.ah != 0x00)
    printf("VESA function call %d %d NOT sucessful\n", class, func);
}
  
vgaIObase()
{
  (inp(0x3cc) & 0x01) ? 0x3d0 : 0x3b0;
}


main ()
{
  struct videoconfig vc;
  int i,j, temp;
  union REGS inregs, outregs;

#ifdef NEVER
  outp(0x3bf, 3);		/* Set the key. */
  outp(vgaIObase() + 8, 0xa0);
  outp(vgaIObase() + 4, 0x11);
  temp = inp(vgaIObase() + 5);
  outp(vgaIObase() + 5, temp &0x7f);

  /* Set the color palette */
  outp(0x3c6, 0xff);
  outp(0x3c8, 0x00);
  for (i=0; i< 768; i++) outp(0x3c9, 0xaa);

  clockselect(0);		/* Start the clock low */

  TS_outp( 0, 0x3 );		/* Sync reset */
  TS_outp( 1, 0x1 );		/* TS mode */
  TS_outp( 2, 0xf );		/* Write pin mask */
  TS_outp( 3, 0x0 );		/* Font sel. */
  TS_outp( 4, 0xe );		/* Mem. mode */

  TS_outp( 6, 0x0 );		/* TS stat. */
  TS_outp( 7, 0xbc);		/* TS aux mode */

  for (i = 0; i < 0x10; i++)
    ATC_outp(i, i);
  ATC_outp(0x10, 0x1 );
  ATC_outp(0x11, 0x0 );
  ATC_outp(0x12, 0xf );
  ATC_outp(0x13, 0x0 );
  ATC_outp(0x14, 0x0 );
  ATC_outp(0x16, 0x0 );


#ifdef NEVER
  CRTC_outp( 0, 0x99);		/* Horiz tot. */
  CRTC_outp( 1, 0x7f);		/* Hor dis end */
  CRTC_outp( 2, 0x7f);		/* Hor blink start */
  CRTC_outp( 3, 0x1d);		/* blink end */
  CRTC_outp( 4, 0x83);		/* Hor sync. start */
  CRTC_outp( 5, 0x17);		/* Hor sync end */
  CRTC_outp( 6, 0x2f);		/* Vert tot */
  CRTC_outp( 7, 0xf5);		/* Ofl low */
  CRTC_outp( 8, 0x0 );		/* Init row adr */
  CRTC_outp( 9, 0x60);		/* Max row adr */
  CRTC_outp(0xa, 0x0);		/* Curs start */
  CRTC_outp(0xb, 0x0);		/* Curs end */
  CRTC_outp(0xc, 0x0);		/* LIn start mid */
  CRTC_outp(0xd, 0x0);		/* Lin start low */
  CRTC_outp(0xe, 0x0 );		/* Curs mid */
  CRTC_outp(0xf, 0x0 );		/* Curs low */
  CRTC_outp(0x10, 0x8 );	/* vrt sync start */
  CRTC_outp(0x11, 0x80);	/* vrt sync end */
  CRTC_outp(0x12, 0xff);	/* vrt dis end */
  CRTC_outp(0x13, 0x80);	/* row offs */
  CRTC_outp(0x14, 0x60);	/* ul row */
  CRTC_outp(0x15, 0xff);	/* vrt blink strt */
  CRTC_outp(0x16, 0x30);	/* vrt blink end */
  CRTC_outp(0x17, 0xAB);	/* CRTC mode */
  CRTC_outp(0x18, 0xff);	/* line comp */
  CRTC_outp(0x33, 0x0);		/* ext strt add */
  CRTC_outp(0x35, 0x80);	/* ofl hi */
#endif

#ifdef NEVER
  CRTC_outp( 0, 0xa1);		/* Horiz tot. */
  CRTC_outp( 1, 0x7f);		/* Hor dis end */
  CRTC_outp( 2, 0x80);		/* Hor blink start */
  CRTC_outp( 3, 0x40);		/* blink end */
  CRTC_outp( 4, 0x88);		/* Hor sync. start */
  CRTC_outp( 5, 0x9e);		/* Hor sync end */
  CRTC_outp( 6, 0x16);		/* Vert tot */
  CRTC_outp( 7, 0xfd);		/* Ofl low */
  CRTC_outp( 8, 0x0 );		/* Init row adr */
  CRTC_outp( 9, 0x60);		/* Max row adr */
  CRTC_outp(0xa, 0x0);		/* Curs start */
  CRTC_outp(0xb, 0x0);		/* Curs end */
  CRTC_outp(0xc, 0x0);		/* LIn start mid */
  CRTC_outp(0xd, 0x0);		/* Lin start low */
  CRTC_outp(0xe, 0x0 );		/* Curs mid */
  CRTC_outp(0xf, 0x0 );		/* Curs low */
  CRTC_outp(0x10, 0x8 );	/* vrt sync start */
  CRTC_outp(0x11, 0x8a);	/* vrt sync end */
  CRTC_outp(0x12, 0xff);	/* vrt dis end */
  CRTC_outp(0x13, 0x80);	/* row offs */
  CRTC_outp(0x14, 0x60);	/* ul row */
  CRTC_outp(0x15, 0x04);	/* vrt blink strt */
  CRTC_outp(0x16, 0x22);	/* vrt blink end */
  CRTC_outp(0x17, 0xAB);	/* CRTC mode */
  CRTC_outp(0x18, 0xff);	/* line comp */
  CRTC_outp(0x33, 0x0);		/* ext strt add */
  CRTC_outp(0x35, 0x00);	/* ofl hi */


  GDC_outp(0x0, 0x0 );
  GDC_outp(0x1, 0x0 );
  GDC_outp(0x2, 0x0 );
  GDC_outp(0x3, 0x0 );
  GDC_outp(0x4, 0x0 );
  GDC_outp(0x5, 0x40 );
  GDC_outp(0x6, 0x5 );
  GDC_outp(0x7, 0xf );
  GDC_outp(0x8, 0xff );
#endif

#include "asdasd"
#endif

  vesacall(2, 0x105);

  getch();

  DisplayWidth = 1024;
  DisplayHeight = 768;

  /* Measure the image to be drawn and allocate memory for it. */
  tmpbuffer = (unsigned char *)halloc( DisplayWidth * DisplayHeight, sizeof( char ));

  if ( tmpbuffer == (unsigned char *)NULL ) {
    printf("Noop\n");
    exit( 1 );
  }

  for (i = 0; i < ((DisplayWidth * DisplayHeight)); i++)
    tmpbuffer[i]= i % 256;

  DisplayWidth32 = DisplayWidth / 32;
  DisplayWidth8 = DisplayWidth / 8;


  DOSbitbli(tmpbuffer,0,0, DisplayWidth, DisplayHeight);

  getch();

  for (i = 0; i < DisplayHeight; i++) {
    for (j = 0; j < DisplayWidth ; j++) {
      inregs.h.ah = 0xc;
      inregs.h.al = i % 256;
      inregs.x.cx = i;
      inregs.x.dx = j;
      int86( 0x10, &inregs, &outregs );
    };
  };
  getch();

  _setvideomode( _DEFAULTMODE );

  printf("AL = %d\nAH = %d\nBH = %d", outregs.h.al, outregs.h.ah, outregs.h.bh);
}
