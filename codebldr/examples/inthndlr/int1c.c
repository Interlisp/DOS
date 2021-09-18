/*=============================================================================
 * Intel 386/486 C Code Builder(TM) Kit
 * Copyright 1991 Intel Corporation.  All Rights Reserved.
 *-----------------------------------------------------------------------------
 *
 * INT1C.C
 *
 * This program demonstrates the use of an interrupt routine to hook onto the
 * timer interrupt (1Ch).  The interrupt is a protected-mode routine which is
 * invoked PRIOR TO calling the real mode interrupt routine.
 *
 * An interrupt must always be declared using '#pragma interrupt'.  It is
 * installed by using '_dos_setvect()'.  The program installing the interrupt
 * should always save the existing interrupt address, and restore it upon pro-
 * gram termination.
 *
 * A timer interrupt occurs 18.2 times per second (unless it is re-programmed
 * to occur at a different rate).  This program sets the timer interrupt (0x1c)
 * vector to a handler which counts the interrupts (ticks), and sets a flag
 * ('one_sec') each time 1 second elapses.  The main (foreground) program
 * simply loops, looking for a keystroke to terminate the program, and dis-
 * plays the current time & date each time 'one_sec' is set.
 *****************************************************************************/

/******************************************************************************
*  Include files
******************************************************************************/
#include <i32.h>                       /* "#pragma interrupt" & '_chain_intr'*/
#include <dos.h>                       /* defines REGS & other structs       */
#include <stdio.h>                     /* define NULL                        */
#include <stdlib.h>
#include <time.h>


/******************************************************************************
*  Defines
******************************************************************************/
#define FALSE     0
#define TRUE      1
#define OFF       0                    /* OFF, ON are used to call 'Cursor()'*/
#define ON        1

#define TD_COL    60                   /* display time on right hand side    */


/******************************************************************************
*   Functions within this file
******************************************************************************/
void     Cursor(int on_or_off);        /* turn hardware cursor on or off     */
void     Move_cursor(int col, int row);/* move hardware cursor               */
#pragma interrupt(Timer)
void     Timer(void);                  /* int 0x1c interrupt (counts ticks)  */

/******************************************************************************
*   Global variables
******************************************************************************/
int      cursor_bgn, cursor_end;       /* orig cursor's begn & end scan lines*/
int      extra_tick_count = 91;        /* add 1 tick every 18.2*5 ticks      */
int      one_sec = TRUE;               /* set to true by int 1a handlr ea sec*/
void   (*prev_int_1c)();               /* keeps address of previous 1c handlr*/
                                       /* used for chaining & restore at exit*/
int      tick_count = 18;              /* approx 18 ticks per sec            */



/******************************************************************************
*  main()
*
*  This program uses '_dos_getvect()' to retrieve the address of the current
*  interrupt 0x1c vector.  It then calls '_dos_setvect()' to set the 0x1c
*  interrupt vector to 'Timer()', a routine defined in this file.
*
*  Once the 'Timer()' routine has been installed as the current int 0x1c
*  handler, it will set the flag 'one_sec' to TRUE each second.  This program
*  ('main()') constantly checks this flag, resetting it when it is found to be
*  TRUE, and displaying the current time & date.  The program terminates as
*  soon as a keystroke occurs.  Before exiting, the previous 0x1c interrupt
*  vector is restored.
******************************************************************************/
main()
{
   int         len;                    /* used to add string terminatr ('\0')*/
   union REGS  regs;
   time_t      td;                     /* holds current time/date (in binary)*/
   char       *td_str;                 /* ASCIIZ current time/date string    */


/******************************************************************************
*  All code and data touched during the processing of an interrupt should
*  locked prior to receiving any interrupts.  This prevents the Timer
*  function from being swapped out during an interrupt.
******************************************************************************/
   _dpmi_lockregion((void *)&extra_tick_count, sizeof(extra_tick_count));
   _dpmi_lockregion((void *)tick_count, sizeof(tick_count));
   _dpmi_lockregion((void *)one_sec, sizeof(one_sec));
   _dpmi_lockregion((void *)&Timer, 4096);
   
/******************************************************************************
*  Turn off cursor (1st use int 0x10, func 3 to get the current cursor shape)
******************************************************************************/
   regs.h.ah = 3;                      /* save original cursor shape         */
   regs.h.bh = 0;                      /* page #                             */
   int86(0x10, &regs, &regs);
   cursor_bgn = regs.h.ch;             /* save old bgn & end cursr scan lines*/
   cursor_end = regs.h.cl;             /* (so we can turn the cursor off)    */

   Cursor(OFF);                        /* turn off cursor for pgm duration   */

/******************************************************************************
*  Clear portion of screen for clock update & display a description
******************************************************************************/
   Move_cursor(0, 0);                  /* home cursor (upper, left corner)   */
                                       /* erase the 1st few lines (easy way!)*/
   printf("                                        "
          "                                        "
          "                                        "
          "                                        ");

   printf("This program displays the time & date once per second.  To do\n"
          "this, it hooks the system timer tick interrupt, which occurs\n"
          "18.2 times per second.\n");
   printf("                                        "
          "                                        "
          "Press any key to quit ....              "
          "                                        "
          "                                        "
          "                                        "
          "                                        "
          "                                        ");

/******************************************************************************
*  Hook int 0x1c interrupt into "high interrupt" (i.e., protected mode) table
******************************************************************************/
   prev_int_1c = _dos_getvect(0x1c);   /* get addr of currnt 1c hndlr, if any*/
   _dos_setvect(0x1c, Timer);          /* hook our int handler to timer int  */

/******************************************************************************
*  Wait until a key is struck; update time each second until keystroke
******************************************************************************/
   while (! kbhit())                   /* continue until a key is hit        */
   {
      if (one_sec)                     /* has 1 second elapsed?              */
      {
         one_sec = FALSE;              /* time to update time - reset flag   */
         td = time(NULL);              /* get current time & date            */
         td_str = ctime(&td);
         len = strlen(td_str) - 6;     /* get rid of the year                */
         td_str[len] = '\0';           /* terminate the string               */
         Move_cursor(TD_COL, 1);       /* move cursor to posn to display time*/
         printf("%s", td_str);         /* display time/date string           */
      }
   }

/******************************************************************************
*  Key pressed, get it; restore original cursor shape & 0x1c vector
******************************************************************************/
   getch();                            /* get key pressed; terminate program:*/
   Move_cursor(0, 8);                  /* move cursor down a bit             */
   _dos_setvect(0x1c, prev_int_1c);    /* unhook our handlr, install previous*/
   Cursor(ON);                         /* restor cursor to its previous state*/
   
/******************************************************************************
*  Unlock the code and data previously locked.
******************************************************************************/
   _dpmi_unlockregion((void *)&extra_tick_count, sizeof(extra_tick_count));
   _dpmi_unlockregion((void *)tick_count, sizeof(tick_count));
   _dpmi_unlockregion((void *)one_sec, sizeof(one_sec));
   _dpmi_unlockregion((void *)&Timer, 4096);
   
   return(0);
}




/*****************************************************************************
*  Cursor(on_or_off)
*
*  Turns the video cursor on or off, as desired.  Avoids unsightly cursor
*  movements by turning off during screen updates.
*
*  Input:
*     on_or_off   -  Defines ON or OFF are used to indicate desired action.
*
*  Input (global):
*     cursor_bgn  -  Original start & stop scan lines of cursor.
*     cursor_end
******************************************************************************/
void  Cursor(int on_or_off)
{
   union REGS  regs;


   regs.h.ah = 0x03;                   /* set cursor type                    */
   regs.h.bh = 0x00;                   /* page 0                             */
   if (on_or_off == ON)
   {
      regs.h.ah = 1;                   /* put back original cursor shape     */
      regs.h.ch = cursor_bgn;
      regs.h.cl = cursor_end;
      int86(0x10, &regs, &regs);
   }
   else                                /* make cursor disappear by shape chng*/
   {
      regs.h.ah = 1;                   /* change shape to an impossible one  */
      regs.h.ch = 0x20;
      regs.h.cl = 0x00;
      int86(0x10, &regs, &regs);
   }
}



/*****************************************************************************
*  Move_cursor(col, row)
*
*  Move the hardware video cursor to the specified row & column using BIOS.
*
*  Input:
*     col   -  Column to move cursor to (0-79).
*     row   -  Row to move cursor to (0-24).
*****************************************************************************/
void  Move_cursor(int col, int row)
{
   union REGS regs;


   regs.h.ah = 2;                      /* 'set cursor position' func         */
   regs.h.bh = 0;                      /* page #                             */
   regs.h.dl = col;
   regs.h.dh = row;

   int86(0x10, &regs, &regs);

   return;
}



/******************************************************************************
*  Timer()
*
*  The interrupt 0x1c handler.  This routine must be declared using the
*  '#pragma interrupt()' statement to ensure that all registers are preserved.
*  It is also needed to ensure the proper functioning of '_chain_intr()'.
*
*  The timer interrupt (normally) occurs 18.2 times per second.  This routine
*  counts each interrupt (tick), and sets 'one_sec' to TRUE after 18 ticks have
*  occurred.  To prevent getting too far behind due to the unaccounted 0.2, it
*  waits one extra tick every 91 ticks (18.2*5).
*
*  Before this interrupt was installed, 'prev_int_1c' was set to the current
*  0x1c interrupt.  'Timer()' chains to this interrupt using '_chain_intr()',
*  rather than returning back to the caller.
*
*  Note that as little as possible should be done within a timer interrupt,
*  since further clock ticks are disabled until the interrupt returns.
******************************************************************************/
void  Timer(void)
{
   if (--extra_tick_count == 0)        /* This is the 91st tick              */
      extra_tick_count = 91;           /* reset back to original 18.2*5 value*/
                                       /* add extra tick by skipping decremnt*/
   else if (--tick_count == 0)
   {
      tick_count = 18;                 /* set back to 18/sec value           */
      one_sec = TRUE;                  /* one second (almost!) has elapsed   */
   }

   _chain_intr(prev_int_1c);           /* call previous int 1c handlr, if any*/
                                       /* (pts to 'ret' if no prev installed)*/
}

