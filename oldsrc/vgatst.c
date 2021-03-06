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
{
  outp( 0x3c0, index);
  outp( 0x3c0, value);
}

main ()
{
  struct videoconfig vc;
  int i,j, reg3cc1, reg3cc2;
  union REGS inregs, outregs;
  reg3cc1 = inp(0x3cc);
  outp(0x3bf, 3);		/* Set the key. */
  if ((inp(0x3cc) & 1) == 1)
    outp(0x3d8, 0xa0);
  else outp(0x3b8, 0xa0);

  outp( 0x3c2, 0x2f );		/* Misc out */

  TS_outp( 0, 0x3 );		/* Sync reset */
  TS_outp( 1, 0x1 );		/* TS mode */
  TS_outp( 2, 0xf );		/* Write pin mask */
  TS_outp( 3, 0x0 );		/* Font sel. */
  TS_outp( 4, 0xe );		/* Mem. mode */

  TS_outp( 6, 0x0 );		/* TS stat. */
  TS_outp( 7, 0xbc);		/* TS aux mode */


  CRTC_outp( 0, 0xa1);		/* Horiz tot. */
  CRTC_outp( 1, 0x7f);		/* Hor dis end */
  CRTC_outp( 2, 0x80);		/* Hor blink start */
  CRTC_outp( 3, 0x04);		/* blink end */
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
  CRTC_outp(0x17, 0xab);	/* CRTC mode */
  CRTC_outp(0x18, 0xff);	/* line comp */
 /* CRTC_outp(0x33, 0x0);	*/	/* ext strt add */
 /* CRTC_outp(0x35, 0x80);	*//* ofl hi */


  GDC_outp(0x0, 0x0 );
  GDC_outp(0x1, 0x0 );
  GDC_outp(0x2, 0x0 );
  GDC_outp(0x3, 0x0 );
  GDC_outp(0x4, 0x0 );
  GDC_outp(0x5, 0x40 );
  GDC_outp(0x6, 0x5 );
  GDC_outp(0x7, 0xf );
  GDC_outp(0x8, 0xff );

  for (i = 0; i < 0x10; i++)
    ATC_outp(i, i);

  ATC_outp(0x10, 0x1 );
  ATC_outp(0x11, 0x0 );
  ATC_outp(0x12, 0xf );
  ATC_outp(0x13, 0x0 );
  ATC_outp(0x14, 0x0 );
  ATC_outp(0x16, 0x0 );

  /* Init the screen to max resolution. */
  inregs.h.ah = 0;		/* Set display mode */
  inregs.h.al = 0xb8;		/* 1024 x 768 and DON'T clear the display*/

#ifdef NEVER
  /* Execute video interrupt: */
  int86( 0x10, &inregs, &outregs );  /* No output from this call. */
#endif

reg3cc2 = inp(0x3cc);
  outp( 0x3cd, 0x0 );		/* Seg select */

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

 /* DOSbitbli(tmpbuffer,0,0, DisplayWidth, DisplayHeight); */

  for (i = 0; i < 16; i++)
    {
      unsigned long *j;
      outp( 0x3cd, i);
      for (j = (unsigned long *)0xa0000; j < (unsigned long *)0xb0000; j++) *j = 0xfefcf8f0;
    }

  getch();

  inregs.h.ah = 0xf;		/* Get video info. */
  int86( 0x10, &inregs, &outregs );
  
  _setvideomode( _DEFAULTMODE );

  printf("AL = %d\nAH = %d\nBH = %d", outregs.h.al, outregs.h.ah, outregs.h.bh);
printf("\n\n 3cc before = 0x%x, after = 0x%x\n", reg3cc1, reg3cc2);
}
