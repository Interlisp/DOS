/* stk.h - DOS extender stack frame
 * $Version: 1.8 $
 * Copyright 1990,91 Intel Corporation, ALL RIGHTS RESERVED.
 *
 * This file contains the structure definition and related constant
 * definitions for the stack frame available to a protected-mode interrupt
 * handler.  A pointer to the stack frame is returned by the function
 * '_get_stk_frame' (defined in <i32.h>).  The stack structure
 * consolidates information from two structures defined by DPMI 0.9: the
 * exception structure (section 10.4), and the call-back structure
 * (section 11).
 */

#ifndef _stkh
#define _stkh

#pragma noalign (_xstack_t)
typedef struct _xstack_t                /* --- DOS Extender Stack Frame --- */
   {                                    /* ================================ */
   unsigned long  rloc;                 /* Relocation factor                */
   unsigned short reserved;             /* Reserved                         */
   unsigned char  opts;                 /* Options                          */
   unsigned char  cc;                   /* Command code                     */
   unsigned long  edi;                  /* Registers of interrupted process */
   unsigned long  esi;                  /*     "     "       "         "    */
   unsigned long  ebp;                  /*     "     "       "         "    */
   unsigned long  tmp;                  /* (Points to global data area-GDA) */
   unsigned long  ebx;                  /*     "     "       "         "    */
   unsigned long  edx;                  /*     "     "       "         "    */
   unsigned long  ecx;                  /*     "     "       "         "    */
   unsigned long  eax;                  /*     "     "       "         "    */

   unsigned short erc;                  /* Error code or reserved           */
   unsigned char  id;                   /* Interrupt ID (software INTs)     */
   unsigned char  idi;                  /* Intel interrupt ID (exceptions)  */

   unsigned long  eip;                  /* Registers                    EIP */
   unsigned long  cs;                   /* of the                        CS */
   unsigned long  flg;                  /* interrupted               EFLAGS */
   unsigned long  esp;                  /* process                      ESP */
   unsigned long  ss;                   /*                               SS */
   unsigned long  es;                   /* V86-mode registers               */
   unsigned long  ds;                   /*  "    "     "                    */
   unsigned long  fs;                   /*  "    "     "                    */
   unsigned long  gs;                   /*  "    "     "                    */
   } _XSTACK;                           /* -------------------------------- */

#define _STK_WRK              8         /* Length of stack work space       */
#define _STK_LEN        sizeof(_XSTACK) /* Length of stack frame            */

                                        /* Stack options (opts field) ----- */
#define _STK_NOINT         0x80         /* Suppress interrupt               */
#define _STK_TERM          0x40         /* Terminate application            */

                                        /* EFLAG Values ------------------- */
#define _FLAG_CARRY      0x0001         /* Carry flag                       */
#define _FLAG_PARITY     0x0004         /* Parity flag                      */
#define _FLAG_AUXCARRY   0x0010         /* Auxillary carry flag             */
#define _FLAG_ZERO       0x0040         /* Zero flag                        */
#define _FLAG_SIGN       0x0080         /* Sign flag                        */
#define _FLAG_TRAP       0x0100         /* Trap flag                        */
#define _FLAG_INTERRUPT  0x0200         /* Interrupt enable flag            */
#define _FLAG_DIRECTION  0x0400         /* Direction flag                   */
#define _FLAG_OVERFLOW   0x0800         /* Overflow flag                    */
#define _FLAG_IOPL       0x3000         /* I/O privilege level mask         */
#define _FLAG_NESTED     0x4000         /* Nested task flag                 */
#define _FLAG_RESUME    0x10000         /* Resume flag                      */
#define _FLAG_VM        0x20000         /* Virtual 8086 mode                */

#endif /* _stkh */

