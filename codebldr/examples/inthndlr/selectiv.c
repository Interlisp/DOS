/*=============================================================================
 * Intel 386/486 C Code Builder(TM) Kit
 * Copyright 1991 Intel Corporation.  All Rights Reserved.
 *-----------------------------------------------------------------------------
 *
 * SELECTIV.C - User Protected-Mode Interrupt Handler
 *              Selective Processing Example
 *
 * This program demonstrates use of a protected-mode interrupt handler to
 * selectively process ordinary interrupts using Int 21h, function 41h
 * (delete file) as an example.  The user handler intercepts calls to the
 * delete file function, and transparently performs other processing prior
 * to passing the function to the real-mode handler.
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
#include <dos.h>
#include <stk.h>

#pragma interrupt (int21)
void int21();               /* User Int 21h protected-mode interrupt handler */

void (*prev_int21)();             /* Previous Int 21h protected-mode handler */


void main ( void )            /* Protected-Mode Selective Processing Example */
   {
   union REGS regs;
   char filename[] = "selectiv.$$$";

   printf ( "Protected-Mode Interrupt Handler "
      "Selective Processing Example\n\n" );

   prev_int21 = _dos_getvect ( 0x21u );     /* Save previous handler pointer */
   _dos_setvect ( 0x21u, int21 );            /* Install user Int 21h handler */

   printf ( "\nUser Int 21h handler has been installed.\n" );

   regs.w.eax = 0x00004100;                          /* Delete file function */
   regs.w.edx = (int) filename;               /* Pointer to ASCIIZ file name */
   intdos ( (const union REGS *)&regs, &regs );        /* Invoke DOS service */

   _dos_setvect ( 0x23u, prev_int21 );           /* Restore previous handler */

   printf ( "\nUser Int 21h handler has been removed.\n" );
   }



void int21 ( void )         /* User Int 21h Protected-Mode Interrupt Handler */
   {
   _XSTACK *ebp;                                    /* Real-mode stack frame */

   ebp = (_XSTACK *) _get_stk_frame();            /* Get stack frame address */

   if (((ebp->flg & _FLAG_VM) == 0)      /* process only if NOT virtual mode */
   && ((ebp->eax & 0x0000FF00) == 0x4100))      /* Check for function = 41h */
      {                                          /* (Delete file function)   */
      bdos ( 0x09, (unsigned) "\n\rQuiet save\n\r$", 0u );
      /*
      Quietly save a backup copy of a file to be deleted:

      Get base path (or current directory) from filename specification.
      if 'base path'\DELETED does not exist
         create 'base path'\DELETED

      Search for last occurrence of files DELETED\DELETED.??? (Keep track
         of highest integer equivelant of filename extension, ie '023' = 23).

      Copy the file specified to DELETED\DELETED.ext, where ext is the string
         equivelant of the highest integer equivelant plus one (ie 23 = '024').

      Update file DELETED\INDEX to contain file copied.
      */
      }
   }



/*

Output of SELECTIV.C

- - - User Protected-Mode Selective Processing Example - - -

C:\SELECTIV

Protected-Mode Interrupt Handler Selective Processing Example


User Int 21h handler has been installed.

Quiet save

User Int 21h handler has been removed.

C:\

*/
