/*=============================================================================
 * Intel 386/486 C Code Builder(TM) Kit
 * Copyright 1991 Intel Corporation.  All Rights Reserved.
 *-----------------------------------------------------------------------------
 *
 * INT1B.C
 *
 * An example Int 1Bh (control-Break) interrupt routine and test program.
 *
 * This program demonstrates how to create and install a protected-mode
 * interrupt handler which will be invoked whenever control-Break is pressed.
 *
 * This program is similar to "INT23.C", the control-C interrupt, with all
 * references to "23" replaced by "1b".
 *
 * BIOS's control-Break interrupt handler, if allowed to execute, adds a
 * control-C in the next available spot within the keyboard buffer.  This
 * results in an Int 23h (control-C) interrupt being generated as soon as the
 * control-C is fetched from the buffer.  This routine intercepts the control-
 * Break interrupt, and prevents the BIOS handler from executing.
 *
 * Since a control-Break normally results in an int 0x23 (control-C) interrupt,
 * it is usually more desirable to simply use one interrupt (Int 23h) to handle
 * both cases.  However, since the Int 23h doesn't occur until the control-C
 * is retrieved from the keyboard buffer, there are times (when the
 * keyboard buffer is full, or not being scanned) when the control-Break would
 * be recognized much sooner.
 *
 * Note that this routine is not installed as an Int 23h handler, and therefore
 * does not prevent a control-C from terminating the program.  It would be
 * simple to prevent this; just install 'Int1b()' as the control-C interrupt.
 * The same interrupt can be assigned to both 1Bh and 23h.  (The '_chain_intr'
 * statement, of dubious value for an 1Bh interrupt anyway, would have to be
 * removed, since the default C Int 23h interrupt terminates the program.)
 *****************************************************************************/

/******************************************************************************
*  Include files & defines
******************************************************************************/
#include <i32.h>                       /* "#pragma interrupt" & '_chain_intr'*/
#include <dos.h>                       /* defines REGS & other structs       */
#include <stdio.h>                     /* define NULL                        */
#include <stk.h>                       /* _XSTACK struct definition          */

#define FALSE        0
#define TRUE         1


/******************************************************************************
*   Global variables
******************************************************************************/
void      (*prev_int_1b)();            /* keeps address of previous 0x1b hdlr*/

/******************************************************************************
*   Functions within this file
******************************************************************************/
#pragma interrupt(Int1b)
void  Int1b(void);                     /* the control-c (int 0x1b) handler   */



/******************************************************************************
*  main()
*
*  A sample program which sets the int 0x1b (control-brk) vector to the example
*  interrupt routine, waits for and echoes all input until <Enter>, then
*  restores the original int 0x1b vector & allows the user to terminate via a
*  control-brk.
******************************************************************************/
main()
{


/******************************************************************************
*  Hook int 0x1b interrupt into "high interrupt" (i.e., protected mode) table
******************************************************************************/
   prev_int_1b = _dos_getvect(0x1b);   /* get addr of currnt 1b hndlr, if any*/
   _dos_setvect(0x1b, Int1b);          /* hook our int handler to interrupt  */

/******************************************************************************
*  Display test's sign-on message
******************************************************************************/
   printf("\n*** Control-Break Interrupt Test ***\n"
     "A control-Break (int 0x1b) interrupt handler has been installed.\n"
     "This handler prevents program termination caused by a control-Break.\n\n"
     "Type in a string as desired, including a control-Break.\n"
     "The control-Break will not terminate the program.\n"
     "(A control-C will, however; be sure to use control-Break!)\n"
     "Waiting for input (press <Enter> to end the test)....\n");

/******************************************************************************
*  Input & echo chars until <Enter>;  If control-Brk interrupt is correctly in
*  place, program will not be terminated by a control-Brk.
******************************************************************************/
   while (TRUE)
   {
      if (getche() == 0x0d)            /* <Enter> terminates test, else ignor*/
         break;
   }

/******************************************************************************
*  Replace our int 0x1b interrupt handler with the previously installed handler
******************************************************************************/
   _dos_setvect(0x1b, prev_int_1b);    /* unhook our handlr, install previous*/

/******************************************************************************
*  Display message to notify control-Brk handler is gone
******************************************************************************/
   printf("\n\nThe control-Break interrupt has been removed.\n"
               "Enter a control-Break now to terminate the program.\n");

/******************************************************************************
*  Input & echo chars (allow control-Brk [or control-C] to terminate program)
******************************************************************************/
   while (TRUE)
      getche();                        /* Loop forever (or until cntrl-c hit)*/
}




/******************************************************************************
*  Int1b
*
*  Int 0x1b (Control-Brk) interrupt.
*
*  This routine is called by the extender before control is passed to the DOS
*  (real mode) int 0x1b interrupt handler.
*
*  As currently written, this routine simply sets a flag which specifies
*  that the DOS int handler NOT be called, thereby ignoring the Control-Brk
*  interrupt.  If needed, this routine could instead perform clean-up
*  operations & terminate execution through DOS function 4c.  Or, it could
*  set a local flag to indicate that a control-c was received, which would be
*  tested later by the application.  (It would still be necessary to set the
*  _STK_NOINT flag to prevent the DOS int handler from being called.)
*
*  Input:
*     An _XSTACK frame (a pointer to which is obtained by calling the routine
*        '_get_stk_frame()').  This frame contains a copy of the registers
*        which would be passed to the V86 (real mode) interrupt handler.  See
*        'STK.H' for the layout of this structure.
*
*  Output:
*     The '_STK_NOINT' flag in the _XSTACK frame is set, bypassing the call to
*     the V86 interrupt handler.
*
*  Note: This routine must be declared using "#pragma interrupt" so that all
*        input registers are preserved.  It is also needed to ensure the
*        proper functioning of '_get_stk_frame()'.  This should be done just
*        before the function prototype (see the top of this program).
******************************************************************************/
void  Int1b(void)
{
   _XSTACK  *ptr;                      /* ptr to the stack frame             */


   ptr = (_XSTACK *)_get_stk_frame();  /* get ptr to the V86 _XSTACK frame   */
   ptr->opts |= _STK_NOINT;            /* set _STK_NOINT to prevent V86 call */

   _chain_intr(prev_int_1b);           /* chain to prev int 1b interrupt     */
                                       /* (pts to 'ret' if no prev installed)*/
}

