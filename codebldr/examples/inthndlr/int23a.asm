             PAGE ,132
             .386
;*=============================================================================
;* Intel 386/486 C Code Builder(TM) Kit
;* Copyright 1991 Intel Corporation.  All Rights Reserved.
;*-----------------------------------------------------------------------------
;*   INT23A.ASM
;*
;*   An example Assembly language int 23h (control-C) interrupt routine &
;*   sample program.
;*
;*   This program demonstrates how to create and install a protected-mode
;*   interrupt handler which will be invoked whenever control-C is pressed.
;*
;* The following commands can be used to create an .OBJ file:
;*    MASM 5.1:   masm /I..\..\inc int23a.asm;
;*    MASM 6.0:   ml /c /Zm /I..\..\inc int23a.asm
;*    Phar Lap:   386asm int23a.asm -i ..\..\inc\
;*    TASM 2.x:   tasm /i..\..\inc int23a.asm
;*
;*   The .OBJ file may then be used to create an .REX and an .EXE file by:
;*      icc -N int23a.obj
;*
;*   Two internal extender service calls are performed within this file.  The
;*   calls are made by placing a request number in AH, an argument in AL, and
;*   calling the routine whose address is given in the 'GDA_SERV' field of the
;*   GDA.
;******************************************************************************
CODE            SEGMENT para public USE32 'CODE'
                ASSUME  cs:CODE,ds:CODE,es:CODE


;******************************************************************************
;*   Include files
;******************************************************************************
        include extender.ah             ;global data area ('GDA_SERV' addr)
        include stk.ah                  ;contains STK image defines


;******************************************************************************
;*   Sample program's data
;******************************************************************************
cntrl_c_msg1:
        DB      0dh,0ah,'*** Control-C Interrupt Test ***',0dh,0ah
        DB      'Type in a string as desired, including a control-C.  The '
        DB      'control-C will',0dh, 0ah
        DB      'not terminate the program.',0dh,0ah
        DB      'Waiting for input....  '
        DB      '(Press <Enter> to terminate.)',0dh, 0ah
CNTRL_C_MSG1_LEN        EQU     $-cntrl_c_msg1

cntrl_c_msg2:
        DB      0dh,0ah,0dh,0ah,'The control-C interrupt handler has '
        DB      'been removed.',0dh,0ah
        DB      'Enter a control-C now to terminate the program.',0dh,0ah
CNTRL_C_MSG2_LEN        EQU     $-cntrl_c_msg2

gda_ptr DD      0                       ;address of gda



;******************************************************************************
;*   test_int23
;*
;*   A sample program which sets the int 23h vector to the example interrupt
;*   handler ('int23'), waits for and echoes all input until <Enter>, then
;*   removes the int 23h vector & allows the user to terminate via a control-C.
;*
;*   Input:
;*      edx     -       Address of the GDA.  (Note:  If this program were to be
;*                      linked with a C module, requiring the C startup code to
;*                      execute first, the gda pointer would not be in edx, but
;*                      it could be retrieved from the global variable '_gda'.)
;******************************************************************************
test_int23:

;******************************************************************************
;*   Hook int 23h interrupt handler into "high interrupt" table
;******************************************************************************
        mov     gda_ptr,edx             ;save ptr to gda
        call    int23_init              ;initialize int 23h interrupt handler

;******************************************************************************
;*   Display test's sign-on message
;******************************************************************************
        mov     ah,40h                  ;write to device
        mov     ebx,1                   ;stdout's fixed handle
        mov     ecx,CNTRL_C_MSG1_LEN    ;size of msg to display
        mov     edx,OFFSET cntrl_c_msg1 ;addr of msg to display
        int     21h

;******************************************************************************
;*   Input & echo chars until <Enter>;  If control-C handler is correctly in
;*      place, program will not be terminated by a control-C.
;******************************************************************************
keep_looping:
        mov     ah,1                    ;char input with echo
        int     21h

        cmp     al,0dh                  ;terminate when <Enter> hit
        jne     keep_looping

;******************************************************************************
;*   Unhook int 23h interrupt handler
;******************************************************************************
        mov     edx,gda_ptr             ;restore ptr to gda
        call    int23_term              ;terminate int 23h interrupt handler

;******************************************************************************
;*   Display message to notify control-C handler is gone
;******************************************************************************
        mov     ah,40h                  ;write to device
        mov     ebx,1                   ;stdout's fixed handle
        mov     ecx,CNTRL_C_MSG2_LEN    ;size of msg to display
        mov     edx,OFFSET cntrl_c_msg2 ;addr of msg to display
        int     21h

;******************************************************************************
;*   Input & echo chars (allow control-C to terminate program)
;******************************************************************************
loop_forever:
        mov     ah,1                    ;char input with echo
        int     21h
        jmp     SHORT loop_forever      ;loop until control-C terminates pgm







;******************************************************************************
;*   All code below belongs to the int 23h interrupt handler template:
;******************************************************************************


;******************************************************************************
;*   Local data area used by interrupt handler routines
;******************************************************************************
int_gda_ptr     DD      0               ;address of gda
old_int23       DD      0               ;address of previous int 23h handler



;******************************************************************************
;*   int23_init
;*
;*   Performs initialization for int 23h handler.  First, a service call
;*   is performed to retrieve the current int 23h handler address, if any.
;*   This address is saved for use by the new handler (to chain to the cur-
;*   rent handler).  The second service call sets the new int 23h handler
;*   routine to be the current routine.
;*
;*   The service calls could be replaced with similar calls to '_dos_getvect()'
;*   and '_dos_setvect()' if this code were linked with the C library.  In
;*   that case, the GDA address would not be needed.
;*
;*   Input:
;*      edx - GDA address.
;*
;*   Output:
;*      edx preserved.  Other registers volatile.
;******************************************************************************
int23_init:
        mov     int_gda_ptr,edx         ;save ptr to gda

;******************************************************************************
;*   Install int 23h (Control-C) interrupt handler
;******************************************************************************
        mov     ah,35h                  ;GET interrupt handler addr
        mov     al,23h                  ;interrupt # to get
        call    [edx].GDA_SERV          ;returns current handler addr in ebx
        mov     old_int23,ebx           ;save existing handler addr, if any

        xor     eax,eax                 ;clear hi bit (set = no V86 ints)
        mov     ah,25h                  ;SET interrupt handler addr
        mov     al,23h                  ;interrupt # to set
        lea     edx,int23               ;get address of our int 23h handler
        mov     ebx,int_gda_ptr         ;get ptr to gda
        call    [ebx].GDA_SERV          ;call service routine to set int addr

;******************************************************************************
;*   Exit 'int23_init'
;******************************************************************************
        mov     edx,int_gda_ptr         ;restore ptr to gda before returning
        ret




;******************************************************************************
;*   int23
;*
;*   Int 23h (Control-C) interrupt handler.
;*
;*   This routine is called before control is given to the DOS (real mode) int
;*   23h interrupt handler.
;*
;*   As currently written, this routine simply sets a flag which specifies
;*   that the DOS int handler not be called, thereby ignoring the Control-C
;*   handler.  If needed, this routine could instead perform clean-up
;*   operations & terminate execution through DOS function 4c.  Or, it could
;*   set a local flag to indicate a control-c was received, which could be
;*   tested later by the application (it would still be necessary to set the
;*   NOINT flag to prevent the DOS int handler from being called).
;*
;*   Input:
;*      ebp     - Address of STK image to be sent to the Virtual 86 interrupt
;*                   handler.  To return a flag value to the interrupted pro-
;*                   cess (if '_STK_NOINT' is set), modify STK_FLG through ebp.
;*      ebx     - Address of STK image showing status at time of interrupt.
;*                   If the interrupted process is a Virtual 86 process, ebx
;*                   will be the same as ebp.  To set a return value in a
;*                   register (if '_STK_NOINT' is set), modify the appropriate
;*                   register (e.g., 'STK_EBX', etc.) through ebx.  The modi-
;*                   fied register value will be returned to the interrupted
;*                   process.
;*                   Note:  Flag changes must be made using ebp - see above.
;*   Output:
;*      All registers must be preserved.
;******************************************************************************
int23:
        or      [ebp].stk_opts,_STK_NOINT ;don't call DOS (V86) int handler
        cmp     old_int23,0             ;was there an "old" int23 handler?
        jne     goto_old_int23          ;if so, continue with old routine
        ret
goto_old_int23:
        jmp     [old_int23]             ;continue int 23 processing (this may
                                        ;NOT be desireable in some cases!)


;******************************************************************************
;*   int23_term
;*
;*   Performs cleanup for the int 23h handler.  A service call is made to
;*   restore the handler that was current before the new handler was
;*   installed.
;*
;*   Input:
;*      edx - GDA address.
;*
;*   Output:
;*      edx preserved.  Other registers volatile.
;******************************************************************************
int23_term:
        mov     int_gda_ptr,edx         ;save ptr to gda

;******************************************************************************
;*   Remove int 23h handler by replacing with previous handler
;******************************************************************************
        mov     ah,25h                  ;SET interrupt handler addr
        mov     al,23h                  ;interrupt # to set
        mov     edx,old_int23           ;get address of old int 23h handler
        mov     ebx,int_gda_ptr         ;get ptr to gda
        call    [ebx].GDA_SERV          ;call service routine to set int addr

;******************************************************************************
;*   Exit 'int23_term'
;******************************************************************************
        mov     edx,int_gda_ptr         ;restore ptr to gda before returning
        ret


CODE    ENDS
        end     test_int23

