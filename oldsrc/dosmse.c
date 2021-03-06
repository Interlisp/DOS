

#include <i32.h>                             /* Defines "#pragma interrupt"  */
#include <dos.h>                             /* Defines REGS & other structs */
#include <stk.h>                             /* _XSTACK struct definition    */
#include "dosmse.h"                        /* Mouse defines                */

/******************************************************************************
* Defines
******************************************************************************/
#define BYTE      unsigned char
#define FALSE     0
#define TRUE      1

/******************************************************************************
* Functions defined in this module
******************************************************************************/
#pragma interrupt(Mouse_hndlr)

void  Mouse_hndlr(void);           /* Fields mouse events from driver        */
                                   /*  (during servicing of mouse interrupt) */

void  Process_mouse_event(void);               /* Processes last mouse event */

/******************************************************************************
* Global data area
******************************************************************************/
unsigned       drvr_flags;          /* Event flags passed by 'Mouse_hndlr'   */
unsigned x,y;				/* X,Y loc of mouse */
BYTE           mouse_event = FALSE; /* TRUE when event ready in 'drvr_flags' */
unsigned short mouse_shape[] =      /* Defines a block cursor in CGA mode    */
   {                                /* 2 16-word arrays passed to function 9 */
   0,0,0,0,0,0,0,0,                 /* Screen mask array (AND with screen)   */
   0,0,0,0,0,0,0,0,                 /*                                       */
   0xffff,0xffff,0xffff,0xffff,     /* Cursor mask                           */
   0xffff,0xffff,0xffff,0xffff,     /*                                       */
   0xffff,0xffff,0xffff,0xffff,     /* XOR with result                       */
   0xffff,0xffff,0xffff,0xffff      /*                                       */
   };
BYTE           old_mode;                 /* Save text mode number upon entry */
BYTE           redisplay;                 /* TRUE to re-display test 2 usage */

/******************************************************************************
* main
*
* Start of the mouse interrupt (0x33) test program.
******************************************************************************/
main()
   {
   union REGS regs;


   /***************************************************************************
   * Reset mouse driver, exit if no mouse driver present
   ***************************************************************************/
   regs.w.eax = 0;                    /* Function 0 = reset the mouse driver */
   int86(0x33, &regs, &regs);

   if (regs.w.eax != -1) {    /* Check if mouse driver installed (-1 if not) */
      printf("Mouse driver must be installed before running this program.\n");
      return(1);
      }


   /***************************************************************************
   * Save current video mode & switch to mode 4
   ***************************************************************************/
   regs.h.ah = 0xf;                                /* Get current video mode */
   int86(0x10, &regs, &regs);
   old_mode = regs.h.al;            /* Save current mode for restore at exit */

   regs.w.eax = 0x4;              /* Function 0 = change mode, 04 = new mode */
   int86(0x10, &regs, &regs);

   /***************************************************************************
   * Turn on existing mouse cursor & wait for key press
   ***************************************************************************/
   regs.w.eax = 0x0001;                 /* Function 1 = turn on mouse cursor */
   int86(0x33, &regs, &regs);
   regs.w.eax = 0x0002;                 /* Function 1 = hide mouse cursor */
  /* int86(0x33, &regs, &regs); */

   if (getch() == 0)                                 /* Wait for a key press */
      getch();                       /* If special character, clear 2nd code */

   /***************************************************************************
   * Change shape of cursor to a block & wait for key press
   ***************************************************************************/
   regs.w.eax = 0x9;               /* Function 9 = set graphics mouse cursor */
   regs.w.ebx = 0x0;                           /* "Hotspot" offset from left */
   regs.w.ecx = 0x0;                            /* "Hotspot" offset from top */
   regs.w.edx = FP_OFF(mouse_shape);         /* Address of cursor shape data */
   int86(0x33, &regs, &regs);

   if (getch() == 0)                                 /* Wait for a key press */
      getch();                       /* If special character, clear 2nd code */

   /***************************************************************************
   * Change back to previous video mode
   ***************************************************************************/
   regs.h.ah = 0x0;        /* Interrupt 0x10, function 0 = change video mode */
   regs.h.al = old_mode;                      /* Restore original video mode */
   int86(0x10, &regs, &regs);

   /***************************************************************************
   * Lock the "user-defined" handler and its data before installing it.
   * This prevents the handler from being "swapped out" at a critical time.
   ***************************************************************************/

   _dpmi_lockregion((void *)&drvr_flags, sizeof(drvr_flags));
   _dpmi_lockregion((void *)&mouse_event, sizeof(mouse_event));
   _dpmi_lockregion((void *)&Mouse_hndlr, 4096);

   /***************************************************************************
   * Install "user-defined" handler for events: left, right button pressed
   ***************************************************************************/
   regs.w.eax = 0x0C;        /* Function 0C = set user-defined mouse handler */
   regs.w.ecx = LB_PRESS | RB_PRESS | CB_PRESS | LB_OFF | RB_OFF;
   regs.w.edx = FP_OFF(Mouse_hndlr); /* Address of our mouse handler routine */
   int86(0x33, &regs, &regs);       /* Install our handler to process events */

   if (regs.w.eax == 'MERR') {       /* Check for error return "code" in eax */
      printf("Unable to install mouse handler - not enough low memory.\n");
      goto mouse_exit;                         /* Uninstall intercept & exit */
      }

   /***************************************************************************
   * Display test usage instructions
   ***************************************************************************/
   printf("The next test installs a \"user\" mouse handler to process\n"
      "mouse events.  The handler executes in protected mode.\n");
   redisplay = TRUE;                    /* Display remainder of instructions */

   /***************************************************************************
   *  Main loop - Wait for keyboard or mouse event
   ***************************************************************************/
   while (TRUE) {
      if (redisplay) {         /* Only first time or back from spawned child */
         redisplay = FALSE;
         printf("Press the left or right button to invoke the handler.\n"
            "   The left button will execute a child process.\n"
            "   The right button will simply display a message.\n"
            "Press <Enter> to stop the test.\n\n");
         }

      if (kbhit()) {                        /* Key "event" - get key pressed */
         if (getch() == 0x0d)        /* <Enter> terminates test, else ignore */
            break;
         }

      if (mouse_event)        /* Check if mouse event signaled by our hndler */
         Process_mouse_event();              /* If yes - process mouse event */
      }

   /***************************************************************************
   * Program exit - Display message & terminate
   ***************************************************************************/
mouse_exit:
   /***************************************************************************
   * unlock the "user-defined" handler and its data.
   ***************************************************************************/

   _dpmi_unlockregion((void *)&drvr_flags, sizeof(drvr_flags));
   _dpmi_unlockregion((void *)&mouse_event, sizeof(mouse_event));
   _dpmi_unlockregion((void *)&Mouse_hndlr, 4096);

   printf("The test has been terminated.\n");
   }

/******************************************************************************
* Process_mouse_event
*
* This routine examines the current mouse event last processed by the user-
* installed mouse handler ('Mouse_hndlr'), and displays a message describing
* the event.
*
* Called by the main routine after a mouse event occurs.  The "user" mouse
* handler signals the main routine that a mouse event has occurred by setting
* 'mouse_event' to TRUE (after saving the event flag(s) in 'drvr_flags').  As
* currently written, if any more mouse events occur before the previous event
* has been processed here, the mouse handler ignores them.
*
* Note:  As written, the handler is only notified when a left or right but-
* ton is pressed, so these are the only events processed by this routine.  To
* service all mouse events, the interrupt handler would need to queue the
* events as they occur.  This routine would then need to service the events
* from the queue.
*
* Input:
*    drvr_flags     -  Event flag(s) representing mouse event(s).
*    mouse_event    -  TRUE.
*
* Output:
*    mouse_event    -  FALSE (indicates 'drvr_flags' has been processed).
******************************************************************************/
void  Process_mouse_event(void)
   {
   BYTE           event_flags;          /* Local copy of global 'drvr_flags' */
   union REGS     regs; int myx, myy;


   event_flags = drvr_flags;             /* Copy flags representing event(s) */
 myx= x & 0xFFFF; myy = y & 0xFFFF;
   mouse_event = FALSE;                /* Let our handler save another event */

   /***************************************************************************
   * Process mouse event: Left button pressed
   ***************************************************************************/
   if ((drvr_flags & LB_PRESS) != 0) {
      printf("You pressed the LEFT button; spawning a child process....\n");
      system("mt2");                              /* Spawn the child process */

      regs.w.eax = 0x0C;                     /* Must re-install since child  */
      regs.w.ecx = LB_PRESS | RB_PRESS;      /*  used its own mouse handler  */
      regs.w.edx = FP_OFF(Mouse_hndlr);      /* Address of our mouse handler */
      int86(0x33, &regs, &regs);             /* Re-install our handler       */

      redisplay = TRUE;                /* Re-display test usage instructions */
      }

   /***************************************************************************
   * Process mouse event: Right button pressed
   ***************************************************************************/
   if ((drvr_flags & RB_PRESS) != 0)
      printf("You pressed the RIGHT button.\n");
   if ((drvr_flags & RB_OFF) != 0)
      printf("You released the RIGHT button.\n");
   if ((drvr_flags & CB_PRESS) != 0)
      printf("You pressed the MIDDLE button. X = %d, Y = %d.\n", myx, myy);

   /***************************************************************************
   * Exit 'Process_mouse_event'
   ***************************************************************************/
   return;
   }

/******************************************************************************
* Mouse_hndlr
*
* This "user-installed" handler is invoked by the mouse driver whenever an
* event occurs which matches one of the events specified when the handler
* was installed.
*
* Note: The handler must be declared using "#pragma interrupt()" in order
* to retrieve the mouse driver's registers from the _XSTACK frame.  Also
* note that the mouse handler should not make any DOS or BIOS interrupt calls,
* either directly, or indirectly through a C library call.
*
* As currently written, the mouse handler is blocked from storing the event
* information if the previous event has not yet been processed by the main
* program.  If new events are added, it may be necessary for the handler to
* queue the mouse events to ensure that none are missed.
*
* Input:
*    _XSTACK - Stack frame containing driver's registers:
*       eax - Mouse event flags, as follows:
*             Bit   Meaning if set
*             ===   ==============
*             0     Mouse movement
*             1     Left button pressed
*             2     Left button released
*             3     Right button pressed
*             4     Right button released
*             5     Center button pressed
*             6     Center button released
*       ebx - Button state, as follows:
*             Bit   Meaning if set
*             ===   ==============
*             0     Left button is down
*             1     Right button is down
*             2     Center button is down
*       ecx - Horizontal (X) mouse pointer coordinate.
*       edx - Vertical (Y) mouse pointer coordinate.
*       esi - Last raw vertical mickey count.
*       edi - Last raw horizontal mickey count.
*    Although all values are stored in 32-bit locations, only the lower
*    16-bits of information contain data from the mouse driver.
*
*  Output:
*    drvr_flags     -  Event flag(s) (from ax) representing mouse event(s).
*    mouse_event    -  TRUE to indicate a new mouse event has occurred.
******************************************************************************/
void  Mouse_hndlr(void)
   {
   _XSTACK  *stk_ptr;
    unsigned short mouse_flags;
    unsigned short locx, locy;
      stk_ptr = (_XSTACK *)_get_stk_frame(); /* Get ptr to V86 _XSTACK frame */
      mouse_flags = stk_ptr->eax;       /* Save event flags from mouse driver */
	x = stk_ptr->ecx; y=stk_ptr->edx;

    if (mouse_flags & LB_PRESS)
	PUTBASEBIT68K( EmRealUtilin68K, MOUSE_LEFT, 0 );
    if (mouse_flags & LB_OFF)
 	PUTBASEBIT68K( EmRealUtilin68K, MOUSE_LEFT, 1 );

    if (mouse_flags & CB_PRESS)
	PUTBASEBIT68K( EmRealUtilin68K, MOUSE_MIDDLE, 0 );
    if (mouse_flags & CB_OFF)
	PUTBASEBIT68K( EmRealUtilin68K, MOUSE_MIDDLE, 1 );

    if (mouse_flags & RB_PRESS)
	PUTBASEBIT68K( EmRealUtilin68K, MOUSE_RIGHT, 0 );
    if (mouse_flags & RB_OFF)
 		PUTBASEBIT68K( EmRealUtilin68K, MOUSE_RIGHT, 1 );

    if (mouse_flags & MOUSE_MV)
      {
	/* The mouse has moved; track it. */
#ifndef OLD_CURSOR
	if(!ScreenLocked)
#endif
	  { ScreenLocked=T;
	    locx = stk_ptr->ecx & 0xFFFF;
	    locy = stk_ptr->edx & 0xFFFF;
	    MouseMove(locx, locy);
	    TrackCursor(locx,locy);
	    ScreenLocked=NIL;
	  }
      }

   return;
   }

