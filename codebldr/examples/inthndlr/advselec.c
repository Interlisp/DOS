/*=============================================================================
 * Intel 386/486 C Code Builder(TM) Kit
 * Copyright 1991 Intel Corporation.  All Rights Reserved.
 *-----------------------------------------------------------------------------
 *
 * ADVSELEC.C - Advanced User Protected-Mode Interrupt Handler
 *              Selective Processing Example
 *
 *
 * This program demonstrates use of a protected-mode interrupt handler to
 * selectively process ordinary interrupts that modify registers returned to
 * the interrupted process.  This example modifies the operation of Int 21h,
 * function 41h, delete file, as an example.  The modification is to prohibit
 * the use of this function to delete files.  Thus, on entry to the Int 21h
 * handler, if function 41h is specified, the error value of 85h is placed
 * into the AX register, the carry flag is set and control is returned to the
 * caller.
 *
 * As discussed in CTLCINT.C, the EBP register contains a pointer to the
 * stack frame that is passed to the real-mode handler.  Similarly, the EBX
 * register contains a pointer to the stack frame that is returned to the
 * calling process.  The function _get_stk_frame can be used from a C
 * program to obtain the EBP register contents.  To obtain the contents of
 * the EBX register, an assembler routine is provided.  (See the end of this
 * description for an alternative if an assembler is not available).
 *
 * Through references to EBX, register values to be returned to the caller
 * can be modified.  The stack frame structure in the file STK.H is relevant
 * to both the EBP and the EBX stack frame.
 *
 * The output of this program is appended to the end of this file.
 *
 * Alternative method for obtaining EBX register value:
 * If an assembler is not available, but access to return registers is
 * necessary, a small "in-line routine" can be used.  This routine will
 * return the value of the EBX register pushed on the stack on entry to
 * the handler.  Although the actual assembler routine is reccommended,
 * this method may suit your purpose.
 * The algorithm follows:
 *
 *    Set up a 32-bit memory location (ie, an int).
 *    Place the hexadecimal value C310458B in this location.
 *    Set up a pointer that can be referenced as a function
 *       to point at the base of this memory location.
 *    Reference this pointer as a function and cast its return as int.
 *
 * The 32-bit value represents a small piece of assembly language as follows:
 *    8B4510   MOV eax, [ebp+10h]   ; Move ebx (from stack) to eax
 *    C3       RET                  ; Return eax
 *
 * The declarations and code for this method follow:
 *
 *    int _get_ebx_text = 0xC310458B;       Assembly 'routine'
 *    int (*_get_ebx)();                    Pointer to assembly routine
 *
 *    (int *) _get_ebx = &_get_ebx_text;    Set pointer to _get_ebx routine
 *    ebx = (_XSTACK *) (_get_ebx)();       Get ebx pointer
 *
 * The value returned will be the value of the EBX register on entry to the
 * handler.
 *
 *---------------------------------------------------------------------------*/


#include <stdio.h>
#include <dos.h>
#include <stk.h>

#pragma interrupt (int21)
void int21();               /* User Int 21h protected-mode interrupt handler */

void (*prev_int21)();             /* Prevoius Int 21h protected-mode handler */
int _get_ebx ( void );              /* Returns the value of the ebx register */


void main ( void ) /* Advanced User Protected-Mode Interrupt Handler Example */
   {
   union REGS regs;
   char filename[] = "ADVSELEC.EXE";

   printf ( "Advanced User Protected-Mode Interrupt Handler Example\n\n" );

   prev_int21 = _dos_getvect ( 0x21u );     /* Save previous handler pointer */
   _dos_setvect ( 0x21u, int21 );            /* Install user Int 21h handler */

   printf ( "\nUser Int 21h handler has been installed.\n\n" );

   regs.w.eax = 0x00004100;                          /* Delete file function */
   regs.w.edx = (int) filename;      /* Pointer to ASCIIZ file specification */
   intdos ( (const union REGS *) &regs, &regs );       /* Invoke DOS service */

   if ( regs.x.cflag )                         /* Check if an error occurred */
      switch ( regs.x.ax )
         {
         case 0x02: printf ( "File not found\n" );                       break;
         case 0x05: printf ( "Access denied\n" );                        break;
         case 0x85: printf ( "Application prohibits deleting files\n" ); break;
         default: printf ( "Unknown error (ax = %.4X)\n", regs.x.ax );
         }

   _dos_setvect ( 0x23u, prev_int21 );           /* Restore previous handler */

   printf ( "\nUser Int 21h handler has been removed.\n" );
   }



void int21 ( void )         /* User Int 21h Protected-Mode Interrupt Handler */
   {
   _XSTACK *ebp;                  /* Stack frame passed to real-mode handler */
   _XSTACK *ebx;              /* Stack frame returned to interrupted process */

   ebp = (_XSTACK *) _get_stk_frame();            /* Get stack frame address */

   if ( ebp->flg & _FLAG_VM )           /* do not process if in virtual mode */
      return;

   if ((ebp->eax & 0x0000FF00) != 0x4100)        /* Check for function = 41h */
      return;

   ebx = (_XSTACK *) _get_ebx();           /* Get return stack frame address */

   ebx->eax = 0x00000085;                   /* Deleting files is not allowed */

   ebp->flg |= _FLAG_CARRY;           /* Set carry flag to indicate an error */
   ebp->opts |= _STK_NOINT;                      /* Bypass real-mode handler */
   }



/*

Output of ADVSELEC.C

- - - Advanced User Protected-Mode Interrupt Handler Example - - -

C:\ADVSELEC

Advanced User Protected-Mode Interrupt Handler Example

User Int 21h handler has been installed.

Application prohibits deleting files

User Int 21h handler has been removed.

C:\

*/

