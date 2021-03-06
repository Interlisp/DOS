#include "dbprint.h"
#include "mouseif.h"
#include "lispemul.h"

MouseInterfaceRec _curmouse;
MouseInterface currentmouse = &_curmouse;

make_mouse_instance( mouse )
MouseInterface mouse;
{
  mouse->LastCursorX = 0;
  mouse->LastCursorY = 0;
  mouse->LastCursorClippingX = 0;
  mouse->LastCursorClippingY = 0;
#ifdef XWINDOW
  mouse->set_cursor_position = &set_Xmouseposition;
#elif DOS
  mouse->set_cursor_position = &set_DOSmouseposition;
  mouse->motion_event = &DOSMouseMotionEvent;
  mouse->button_event = &DOSMouseButtonEvent;
#endif /* if XWINDOW else DOS */
}

DOSMouseButtonEvent(mouse, button, upflag)
     MouseInterface mouse;
{
  register u_int  upflg;
  int kn;
  DLword w,r;
  KBEVENT *kbevent;

  if ((event & 0xCC00) == 0xCC00) { upflg = event & 1; event &= 0xfffffe; }
  else { upflg = (event > 0x80);  event &= 0x7f; }

  switch (event) {
  case MS_LEFT:			/*mouse_button( MOUSE_LEFT, upflg );*/
    PUTBASEBIT68K( EmRealUtilin68K, MOUSE_LEFT, upflg );
    break;
  case MS_MIDDLE:		/*mouse_button( MOUSE_MIDDLE, upflg );*/
    PUTBASEBIT68K( EmRealUtilin68K, MOUSE_MIDDLE, upflg );
    break;
  case MS_RIGHT:		/*mouse_button( MOUSE_RIGHT, upflg );*/
    PUTBASEBIT68K( EmRealUtilin68K, MOUSE_RIGHT, upflg );
    break;
  }
  r=RING_READ(CTopKeyevent);
  w=RING_WRITE(CTopKeyevent);

  if(r==w) goto KBnext;		/* event queue FULL */

  kbevent=(KBEVENT*)(CTopKeyevent+ w);

  kbevent->W0= (*EmKbdAd068K);	/* Emxxxx do not use GETWORD */
  kbevent->W1= (*EmKbdAd168K);
  kbevent->W2= (*EmKbdAd268K);
  kbevent->W3= (*EmKbdAd368K);
  kbevent->W4= (*EmKbdAd468K);
  kbevent->W5= (*EmKbdAd568K);
  kbevent->WU= (*EmRealUtilin68K);

  if(r==0)			/* Queue was empty */ ((RING*)CTopKeyevent)->read=w;
  if(w >= MAXKEYEVENT) ((RING*)CTopKeyevent)->write = MINKEYEVENT;
  else ((RING*)CTopKeyevent)->write = w + KEYEVENTSIZE;

 KBnext:
  if(*KEYBUFFERING68k ==NIL) *KEYBUFFERING68k=ATOM_T;

  /* For DOS, we're not using getsignaldata, so this has to happen here. */
  KBDEventFlg++;
  Irq_Stk_End=0;
  Irq_Stk_Check=0;
}

void  Mouse_hndlr(void)
{
  _XSTACK  *stk_ptr;
  unsigned short mouse_flags;
  DLword locx, locy;

  /* First save the stack frame. */
  stk_ptr = (_XSTACK *)_get_stk_frame(); /* Get ptr to V86 _XSTACK frame */

  /* Stackframe saved, now we can test for initialization. */
  if (!DOSmouseON) return;

  mouse_flags = stk_ptr->eax;	/* Save event flags from mouse driver */

  if (mouse_flags & MOUSE_MV) {
    (currentmouse->motion_event)(currentmouse);
  }
  else {
    (currentmouse->button_event)(currentmouse);
  }
}

  if (mouse_flags & LB_PRESS) kb_event( MS_LEFT );
  if (mouse_flags & LB_OFF) kb_event( MS_LEFT | 1 );

  if (mouse_flags & CB_PRESS) kb_event( MS_MIDDLE | 0 );
  if (mouse_flags & CB_OFF) kb_event( MS_MIDDLE | 1 );

  if (mouse_flags & RB_PRESS) kb_event( MS_RIGHT | 0 );
  if (mouse_flags & RB_OFF) kb_event( MS_RIGHT | 1 );

  if (mouse_flags & MOUSE_MV)
    {
      /* The mouse has moved; track it. */
#ifndef OLD_CURSOR
      if(!ScreenLocked)
#endif
	{ ScreenLocked=T;
	  TPRINT(("Enter Mouse_hndlr\n"));
	  locx = stk_ptr->ecx & 0xFFFF;
	  locy = stk_ptr->edx & 0xFFFF;
	  {*((DLword *)EmMouseX68K) = locx;
	   *((DLword *)EmMouseY68K) = locy;}
	  {*CLastUserActionCell68k = MiscStats->secondstmp;
	   taking_mouse_down();
	   taking_mouse_up(locx,locy);
	   *EmCursorX68K= locx;
	   *EmCursorY68K= locy;}
	  TPRINT(("Exit Mouse_hndlr\n"));
	  ScreenLocked=NIL;
	}
    }    {
      /* The mouse has moved; track it. */
#ifndef OLD_CURSOR
      if(!ScreenLocked)
#endif
	{ ScreenLocked=T;
	  TPRINT(("Enter Mouse_hndlr\n"));
	  locx = (DLword)stk_ptr->ecx & 0xFFFF;
	  locy = (DLword)stk_ptr->edx & 0xFFFF;
	  IOPage68K->dlmousex = locx;
	  IOPage68K->dlmousey = locy;
	  *CLastUserActionCell68k = MiscStats->secondstmp;
	  if(DisplayInitialized) {
	    register DLword	*srcbase, *dstbase;
	    static int sx, dx, w, h, srcbpl, dstbpl, backwardflg=0;
	    static int src_comp=0, op=0, gray=0, num_gray=0, curr_gray_line=0;

	    /* restore saved image */
	    sx=0;

	    if(MonoOrColor == MONO_SCREEN)
	      {
		dx = currentmouse->LastCursorX; /* old x */
		srcbase=MonoCursor_savebitmap;
		dstbase=DisplayRegion68k +( (currentmouse->LastCursorY) * DLWORD_PERLINE); /* old y */
		w = currentmouse->LastCursorClippingX; /* Old clipping */
		h = currentmouse->LastCursorClippingY;
		srcbpl =HARD_CURSORWIDTH;
		dstbpl =displaywidth;
	      }
	    else
	      {
		dx = currentmouse->LastCursorX * COLOR_BITSPER_PIXEL; /* old x */
		srcbase=ColorCursor_savebitmap;
		dstbase= ColorDisplayRegion68k +
		  ((currentmouse->LastCursorY) * DLWORD_PERLINE * COLOR_BITSPER_PIXEL); /* old y */
		w = currentmouse->Lastfgrep CursorClippingX * COLOR_BITSPER_PIXEL; /* Old clipping */
		h= currentmouse->LastCursorClippingY;
		srcbpl =HARD_CURSORWIDTH * COLOR_BITSPER_PIXEL;
		dstbpl =displaywidth * COLOR_BITSPER_PIXEL ;
	      }
	    op=0;
	    new_bitblt_code;
#ifdef DISPLAYBUFFER
	    if( MonoOrColor == MONO_SCREEN )
	      flush_display_region(dx, currentmouse->LastCursorY, w, h);
#endif
	    /* save hidden bitmap */
	    cursor_hidden_bitmap(locx,locy);
	    /* Copy Cursor Image */
#ifndef INIT
	    copy_cursor(locx,locy);
#endif 
	  }

	  IOPage68K->dlcursorx = currentmouse->LastCursorX = locx;
	  IOPage68K->dlcursorx = currentmouse->LastCursorY = locy;
	  TPRINT(("Exit Mouse_hndlr\n"));
	  ScreenLocked=NIL;
	}
    }

  return;
}
