/* This is the display interface  */


#include <stdio.h>
#include "dbprint.h"
#include "dspif.h"
#include "lispemul.h"

DspInterfaceRec _curdsp, _coldsp;

DspInterface currentdsp = &_curdsp;
DspInterface colordsp = &_coldsp;

#ifdef XWINDOW
extern int LispDisplayRequestedWidth;
extern int LispDisplayRequestedHeight;
#endif /* XWINDOW */

#ifdef DOS
extern int dosdisplaymode;
#endif /* DOS */

make_dsp_instance( dsp, lispbitmap, width_hint, height_hint, depth_hint )
     DspInterface dsp;
     char *lispbitmap;
     int width_hint, height_hint, depth_hint;
{
#ifdef DOS

  TPRINT(("Enter make_dsp_instance, dosdisplaymode is: %d\n", dosdisplaymode));

  if (depth_hint == 0) depth_hint = 1;

  switch(dosdisplaymode) {
  case 1:
    VGA_init( dsp ,0, 0, 0, depth_hint );
    break;
  case 0x102:
  case 0x104:
    VESA_init( dsp ,0, 0, 0, depth_hint );
    break;
  default:
    if (VESA_p()) {
      VESA_init( dsp ,0, 0, 0, depth_hint );
    } else if (VGA_p()){    
      VGA_init( dsp ,0, 0, 0, depth_hint );
    } else {			/* Can't set *ANY* video mode! */
      (void)fprintf(stderr, "No portable graphics mode supported by this host.\n");
      (void)fprintf(stderr, "\n-Expected VESA or VGA.\n");
      exit(1);
    }
    break;
  }

#elif XWINDOW
  /* lispbitmap is 0 when we call X_init the first time. */
  if ( !X_init( dsp, 0, LispDisplayRequestedWidth, LispDisplayRequestedHeight, 0, depth_hint )) {
    fprintf(stderr, "Can't open display.");
    exit(-1);
  }
#endif				/* DOS | XWINDOW */
}				/* Now we know the Maxi-MooM capabillities of the hardware. */

#ifdef DOS
VESA_p()
{
  /* Magic. Do a vesa call to determine the current mode. */
  return(VESA_call( 3 , 0 ));
}

VGA_p()
{
  return( TRUE );
}
#endif /* DOS */

/*********************************************************************/
/*                                                                   */
/*		     G e n e r i c R e t u r n T                     */
/*                                                                   */
/* Utility function that just returns T                              */
/*                                                                   */
/*********************************************************************/
unsigned long
GenericReturnT()
{
  return(T);
}

void GenericPanic( dsp )
     DspInterface dsp;
{
  TPRINT(("Enter GenericPanic\n"));
  fprintf( stderr, "Panic! Call to uninitialized display slot!" );
  exit(0);
}


SwitchDisplay( display )
     LispPTR display;
{
  DspInterface tmp;		/* Switch-a-roo! */

  describedsp(currentdsp);
  describedsp(colordsp);

  TPRINT(("Enter SwitchDisplay\n"));
  tmp = currentdsp;
  currentdsp = colordsp;
  colordsp = tmp;
  TPRINT(("Exit SwitchDisplay\n"));
  return( display );
}

describedsp( dsp )
     DspInterface dsp;
{
  if ( dsp == 0 ){
    printf("describedsp: Not a dsp!\n");
    return;
  }
  if ( dsp == &_curdsp )
    printf("dsp is B/W display\n");
  else
    printf("dsp is COLOR display\n");

  printf("width= %d\n", dsp->displaywidth);
  printf("height= %d\n", dsp->displayheight);
  printf("bitsperpixel= %d\n", dsp->bitsperpixel);
  printf("colors= %d\n", dsp->colors);
  printf("graphicsmode= %d\n", dsp->graphicsmode);
  printf("numberofbanks= %d\n", dsp->numberofbanks);
  printf("BytesPerLine= %d\n", dsp->BytesPerLine);
  printf("DisplayStartAddr= %d\n", dsp->DisplayStartAddr);
#ifdef NEVER
  printf("\n--More--\n");getch();
#endif /* DOS */
  printf("bitblt_to_screen= %d\n", dsp->bitblt_to_screen);
  printf("cleardisplay= %d\n", dsp->cleardisplay);
  printf("enter_display= %d\n", dsp->enter_display);
  printf("exit_display= %d\n", dsp->exit_display);
  printf("before_raid= %d\n", dsp->before_raid);
  printf("after_raid= %d\n", dsp->after_raid);
#ifdef NEVER
  printf("\n--Continue--\n");getch();
#endif /* DOS */
}
