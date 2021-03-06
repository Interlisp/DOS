/*=============================================================================
 * Intel 386/486 C Code Builder(TM) Kit
 * Copyright 1991 Intel Corporation.  All Rights Reserved.
 *-----------------------------------------------------------------------------
 *
 * CTLCINT.C - User Control-C Protected-Mode Interrupt Handler Example
 *
 * This program demonstrates the basic structure of and how to install a
 * user protected-mode interrupt handler using the Control-C interrupt as
 * an example. This user handler simply ignores the Control-C interrupt and
 * and returns to the point of interrupt without passing control to the
 * real-mode DOS handler.
 *
 * Upon entry to the user handler, the EBP register contains the address of
 * a stack frame that will be eventually passed to the real-mode handler.
 * In C, the user handler can obtain the value of the EBP register by using
 * the function _get_stk_frame. This frame, as defined by the structure
 * _XSTACK in the file STK.H, represents the state on entry to the
 * real-mode handler.
 *
 * Through this stack frame, the user handler can modify registers to be
 * passed to the real-mode handler, or specify to bypass the real-mode
 * handler, as in this example.  On return from the user protected-mode
 * handler, the value of the stack frame field 'opts' determines further
 * processing.  The possible values of the field 'opts' are _STK_NOINT and
 * _STK_TERM as defined in STK.H.  _STK_NOINT indicates that control is NOT
 * to be passed to the real-mode handler, but instead, directly back to the
 * interrupted process.  _STK_TERM indicates that the current process is to
 * be terminated.  The default action is to pass control to the real-mode
 * interrupt handler.
 *
 * The state represented by the stack frame is relevant only to the real-
 * mode handler, therefore, in the case that control is NOT passed to the
 * real-mode handler, this stack frame is NOT used (ie, it is NOT passed
 * back to the interrupted process).  Therefore, it is not possible to pass
 * values in registers back to the interrupted process through this stack
 * frame.  Another point to note, this stack frame may be that of either a
 * protected-mode or a real-mode process.  This can be determined by
 * examining the field 'flag'.  The FLAG_VM bit set in this field indicates
 * that the interrupted process was real-mode, otherwise it was protected-
 * mode.  This is important to know when modifying other data in this stack
 * frame because of the differences in address formats (ie segmented vs.
 * flat).  User protected-mode handlers should be designed to handle both
 * interrupts from protected-mode and real-mode processes.
 *
 * The output of this program has been appended to the end of this file.
 *
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include <stk.h>
#include <i32.h>

#pragma interrupt (int23)
void int23();               /* User Int 23h protected-mode interrupt handler */

void (*prev_int23)();             /* Previous Int 23h protected-mode handler */


void main ( void )               /* Control-C Protected-Mode Handler Example */
   {
   printf ( "Control-C Protected-Mode Interrupt Handler Example\n\n" );

   prev_int23 = _dos_getvect ( 0x23u  );    /* Save previous handler pointer */
   _dos_setvect ( 0x23u, int23 );            /* Install user Int 23h handler */

   printf ( "\n\nUser Control-C handler has been installed.\n" );
   printf ( "Control-C will NOT terminate the program.\n");
   printf ( "(Press <ENTER> to continue).\n" );

   while ( getche() != 0x0d );              /* Loop until <ENTER> is pressed */

   _dos_setvect ( 0x23u, prev_int23 );           /* Restore previous handler */

   printf ( "\n\nUser Control-C handler has been removed.\n" );
   printf ( "Control-C will terminate the program.\n");

   while (1)
      getche();                             /* Loop until control-C detected */
   }


void int23 ( void )         /* User Int 23h Protected-Mode Interrupt Handler */
   {
   _XSTACK *ebp;                            /* Real-mode handler stack frame */

   ebp = (_XSTACK *) _get_stk_frame();            /* Get stack frame address */

         /* Perform interrupt handling services, ie, ignore Control-C */

   ebp->opts |= _STK_NOINT;                      /* Bypass real-mode handler */
   }



/*

Output of CTLCINT.C

- - - User Control-C Protected-Mode Interrupt Handler Example - - -

C:\CTLCINT

User Control-C handler has been installed.
Control-C will NOT terminate the program.
(Press <ENTER> to continue).
^C
^C
^C
<ENTER>

User Control-C handler has been removed.
Control-C will terminate the program.
^C

C:\

*/
