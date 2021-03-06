/*=============================================================================
 * Intel 386/486 C Code Builder(TM) Kit
 * Copyright 1991 Intel Corporation.  All Rights Reserved.
 *-----------------------------------------------------------------------------
 *
 * INT23.C
 *
 * An example Int 23h (control-C) interrupt routine and test program.
 *
 * This program demonstrates how to create and install a protected-mode
 * interrupt handler which will be invoked whenever control-C is pressed.
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
void      (*prev_int_23)();            /* keeps address of previous 23 handlr*/

/******************************************************************************
*   Functions within this file
******************************************************************************/
#pragma interrupt(Int23)
void  Int23(void);                     /* the control-c (int 0x23) handler   */



/******************************************************************************
*  main()
*
*  A sample program which sets the int 0x23 (control-c) vector to the example
*  interrupt routine, waits for and echoes all input until <Enter>, then
*  restores the original int 0x23 vector & allows the user to terminate via a
*  control-C.
******************************************************************************/
main()
{


/******************************************************************************
*  Hook int 0x23 interrupt into "high interrupt" (i.e., protected mode) table
******************************************************************************/
   prev_int_23 = _dos_getvect(0x23);   /* get addr of currnt 23 hndlr, if any*/
   _dos_setvect(0x23, Int23);          /* hook our int handler to interrupt  */

/******************************************************************************
*  Display test's sign-on message
******************************************************************************/
   printf("\n*** Control-C Interrupt Test ***\n"
         "A control-C (int 0x23) interrupt handler has been installed.\n"
         "This handler prevents program termination caused by a control-C.\n\n"
         "Type in a string as desired, including a control-C.\n"
         "The control-C will not terminate the program.\n"
         "Waiting for input (press <Enter> to end the test)....\n");

/******************************************************************************
*  Input & echo chars until <Enter>;  If control-C interrupt is correctly in
*  place, program will not be terminated by a control-C.
******************************************************************************/
   while (TRUE)
   {
      if (getche() == 0x0d)            /* <Enter> terminates test, else ignor*/
         break;
   }

/******************************************************************************
*  Replace our int 0x23 interrupt handler with the previously installed handler
******************************************************************************/
   _dos_setvect(0x23, prev_int_23);    /* unhook our handlr, install previous*/

/******************************************************************************
*  Display message to notify control-C handler is gone
******************************************************************************/
   printf("\n\nThe control-C interrupt has been removed.\n"
               "Enter a control-C now to terminate the program.\n");

/******************************************************************************
*  Input & echo chars (allow control-C to terminate program)
******************************************************************************/
   while (TRUE)
      getche();                        /* Loop forever (or until cntrl-c hit)*/
}




/******************************************************************************
*  Int23
*
*  Int 0x23 (Control-C) interrupt.
*
*  This routine is called by the extender before control is passed to the DOS
*  int 0x23 interrupt handler.
*
*  As currently written, this routine simply sets a flag which specifies
*  that the DOS int handler NOT be called, thereby ignoring the Control-C
*  interrupt.  If needed, this routine could instead perform clean-up
*  operations & terminate execution through DOS function 4c.  Or, it could
*  set a local flag to indicate a control-c was received, which would be
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
void  Int23(void)
{
   _XSTACK  *ptr;


   ptr = (_XSTACK *)_get_stk_frame();  /* get ptr to the V86 _XSTACK frame   */
   ptr->opts |= _STK_NOINT;            /* set _STK_NOINT to prevent V86 call */

/* _chain_intr(prev_int_23);           /* call previous int 23 handlr, if any*/
/* Note: The '_chain_intr' is not desirable because C's default 'control-C'
         handler terminates the program, which is exactly what we're trying
         to avoid.                                                           */
   return;
}


