/*=============================================================================
 * Intel 386/486 C Code Builder(TM) Kit
 * Copyright 1991 Intel Corporation.  All Rights Reserved.
 *-----------------------------------------------------------------------------
 *
 * CHAIN.C - User Protected-Mode Interrupt Handler Chaining Example
 *
 * This program demonstrates use of chaining user protected-mode interrupt
 * handlers.  A simple example is presented in which two handlers are
 * chained to the default (or existing) handler.  The first handler prints
 * 'Hello', and then chains to the second handler, which prints "World".  As
 * well-behaved interrupt handlers should perform, control is then chained
 * to the default (or exisiting) handler.
 *
 * Using the function _chain_intr insures that the state on entry to the
 * chained handler is consistent with the state on entry to the chaining
 * interrupt.  In particular, the values of the EBP and EBX registers are
 * preserved.  Thus, the chained interrupt may access these registers if
 * operations need to be performed on either of the stack frames.  Chaining
 * via direct reference to a function pointer is NOT reccommended.  If
 * direct function pointer reference is used, as follows, results may be
 * inconsistent.
 *    *(chained_hanlder)();      Chain to default interrupt-Not Recommended!
 *
 * The output of this program has been appended to the end of this file.
 *
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <dos.h>
#include <stk.h>

#pragma interrupt (hello_interrupt)
void hello_interrupt();                      /* User Int 61h hello_interrupt */
#pragma interrupt (world_interrupt)
void world_interrupt();                      /* User Int 61h world_interrupt */

void (*default_int61)();     /* Default, or existing, protected-mode handler */
void (*prev_int61)();         /* First user protected-mode handler installed */


void main ( void )         /* User Protected-Mode Interrupt Chaining Example */
   {
   union REGS regs;

   printf ( "User Protected-Mode Intercept Chaining Example\n\n" );

   default_int61 = _dos_getvect ( 0x61u );   /* Save default handler pointer */
   _dos_setvect ( 0x61u, world_interrupt );          /* Install user handler */
   printf ( "User Int 61h (world_interrupt) handler has been installed.\n" );

   prev_int61 = _dos_getvect ( 0x61u );     /* Save previous handler pointer */
   _dos_setvect ( 0x61u, hello_interrupt );          /* Install user handler */
   printf ( "User Int 61h (hello_intercept) handler has been installed.\n\n" );

   int86 ( 0x61, (const union REGS *)&regs, &regs );       /* Invoke Int 61h */

   _dos_setvect ( 0x61u, default_int61 );         /* Restore default handler */

   printf ( "\nUser Int 61h (world_interrupt) handler has been removed.\n" );
   printf ( "User Int 61h (hello_interrupt) handler has been removed.\n" );
   }



void hello_interrupt ( void )                /* User hello_interrupt Handler */
   {
   _XSTACK *ebp;                            /* Real-mode handler stack frame */

   ebp = (_XSTACK *) _get_stk_frame();            /* Get stack frame address */
   ebp->opts |= _STK_NOINT;              /* Do not pass to real-mode handler */

   bdos ( 0x09, (unsigned) "Hello $", 0u );               /* Display 'Hello' */

   _chain_intr ( prev_int61 );                  /* Chain to previous handler */
   }


void world_interrupt ( void )                /* User world_interrupt Handler */
   {
   _XSTACK *ebp;                            /* Real-mode handler stack frame */

   ebp = (_XSTACK *) _get_stk_frame();            /* Get stack frame address */
   ebp->opts |= _STK_NOINT;              /* Do not pass to real-mode handler */

   bdos ( 0x09, (unsigned) "World\n\r$", 0u );            /* Display 'World' */

   _chain_intr ( default_int61 );              /* Chain to default intercept */
   }


/*

Output of CHAIN.C

- - - User Protected-Mode Interrupt Chaining Example - - -

C:\CHAIN

User Protected-Mode Interrupt Chaining Example

User Int 61h (world_interrupt) handler has been installed.
User Int 61h (hello_interrupt) handler has been installed.

Hello World

User Int 61h (world_interrupt) handler has been removed.
User Int 61h (hello_interrupt) handler has been removed.

C:\

*/
