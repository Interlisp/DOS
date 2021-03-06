/******************************************************************************
* Intel 386/486 C Code Builder(TM) Kit
* Copyright 1991 Intel Corporation.  All Rights Reserved.
*******************************************************************************
* MT2.C
*
* This program is executed as a child process of "MOUSETST".  It is similar
* to MOUSETST's 2nd test, except only one button is defined as a mouse event.
* Refer to MOUSETST.C for more information.
*
* Since this program and its parent both install a mouse handler, the parent
* must re-install its handler upon return from the child.
******************************************************************************/

/******************************************************************************
* Include files
******************************************************************************/
#include <i32.h>                          /* Defines "#pragma interrupt"     */
#include <dos.h>                          /* Defines REGS & other structures */
#include <stk.h>                          /* _XSTACK struct definition       */
#include "mousetst.h"                     /* MREGS and other defines         */

/******************************************************************************
* Defines
******************************************************************************/
#define BYTE      unsigned char
#define FALSE     0
#define TRUE      1

/******************************************************************************
* External variables (defined in MOUSE.OBJ)
******************************************************************************/

/******************************************************************************
* Functions defined in this module
******************************************************************************/
#pragma interrupt(Mouse_hndlr)
void Mouse_hndlr(void);
void Process_mouse_event(void);

/******************************************************************************
* Data area
******************************************************************************/
unsigned drvr_flags;                   /* Event flags passed by 'Mouse_hndlr'*/
BYTE mouse_event = FALSE;              /* TRUE when an event in 'drvr_flags' */

/******************************************************************************
* main
******************************************************************************/
main()
   {
   int event_ctr = 0;
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
   * Install "user-defined" handler for events: right button pressed ONLY
   ***************************************************************************/
   regs.w.eax = 0x0C;      /* Function 0x0C = Set user-defined event handler */
   regs.w.ecx = RB_PRESS;               /* Call mouse handler on right press */
   regs.w.edx = FP_OFF(Mouse_hndlr);         /* Address of our mouse handler */
   int86(0x33, &regs, &regs);       /* Install our handler to process events */

   if (regs.w.eax == 'MERR') {       /* Check for error return "code" in eax */
      printf("Unable to install mouse handler - not enough low memory.\n");
      goto mouse_exit;                           /* Uninstall handler & exit */
      }

   /***************************************************************************
   * Display test usage instructions
   ***************************************************************************/
   printf("This program uses only the right button to activate the handler.\n"
          "The 3rd press will return you to the parent program.\n\n");

   /***************************************************************************
   * Main loop - Wait for keyboard or mouse event
   ***************************************************************************/
   while (TRUE) {
      if (mouse_event) {     /* Check if mouse event signaled by our handler */
         Process_mouse_event();              /* If yes - process mouse event */
         event_ctr++;
         if (event_ctr == 3)             /* 3rd button press, we're finished */
            break;
         }
      }

   /***************************************************************************
   * Program exit - Display message & terminate
   ***************************************************************************/
mouse_exit:
   printf("Returning to parent program....\n");
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
* Note:  As written, the handler is only notified when a right button is
* pressed, so that is the only event processed by this routine.
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
   BYTE  event_flags;                   /* Local copy of global 'drvr_flags' */

   event_flags = drvr_flags;             /* Copy flags representing event(s) */
   mouse_event = FALSE;                /* Let our handler save another event */

   /***************************************************************************
   * Process mouse event(s)
   ***************************************************************************/
   if ((drvr_flags & LB_PRESS) != 0)                  /* Should not happen ! */
      printf("The LEFT button event should not have appeared!!\n");

   if ((drvr_flags & RB_PRESS) != 0)
      printf("You pressed the RIGHT button.\n");

   return;                                     /* Exit 'Process_mouse_event' */
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

   if (!mouse_event) {           /* Only process if previous event completed */
      stk_ptr = (_XSTACK *)_get_stk_frame(); /* Get ptr to V86 _XSTACK frame */
      drvr_flags = stk_ptr->eax;       /* Save event flags from mouse driver */
      mouse_event = TRUE;           /* Indicate a new mouse event is waiting */
      }

   return;
   }

