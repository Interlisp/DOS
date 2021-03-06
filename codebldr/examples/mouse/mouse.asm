        .386P
;******************************************************************************
;* Intel 386/486 C Code Builder(TM) Kit
;* Copyright 1991 Intel Corporation.  All Rights Reserved.
;******************************************************************************
;* MOUSE.ASM
;*
;* Implements the following interrupt 33h services by means of an intercept:
;*    33H, 09H - Set mouse pointer shape (graphics mode).
;*    33H, 0CH - Set user mouse-event-handler.
;*
;* This file contains the following routines:
;*    _install_33h    - Installs protected mode interrupt handlers (inter-
;*                      cepts) for interrupts 33h & 48h.
;*    deinstall_33h   - Removes the interrupt handlers installed by the above
;*                      routine and re-installs the original handlers.
;*    int33h          - Int 33h interrupt handler.  Provides support for the
;*                      two mouse (33h) services mentioned above.
;*    int48h          - Int 48h interrupt handler.  Used to provide a means
;*                      for 'mouse_bridge' to call a protected mode program.
;*    process_330ch   - Called by 'int33h' to process function 0ch.  Copies
;*                      'mouse_bridge' to low memory and installs it as the
;*                      user mouse handler.
;*    mouse_bridge    - A low-memory, real mode routine which calls the
;*                      actual user's mouse handler routine residing in
;*                      high memory (and executes in protected mode).
;*
;* This file must be linked with the C library routines '_dos_getvect()',
;* _dos_setvect()' (in the standard C library CDOS3F.LIB).  In addition,
;* if you do not link with the standard C library, you must call the
;* function '_cleanup_mouse'(CDOS3F.LIB) to unhook the mouse handler.  
;*
;* The following commands can be used to assemble this file:
;*    MASM 5.1:   masm /Mx /Ml /I..\..\inc mouse.asm;
;*    MASM 6.0:   ml /Zm /c /Cx /Cp /I..\..\inc mouse.asm
;*    Phar Lap:   386asm -twocase -i ..\..\inc\ mouse.asm
;*    TASM 2.x:   tasm /mx /ml /I..\..\inc mouse.asm;
;*
;* The object module produced by this source file is included in the
;* library file MOUSE.LIB as 'cq_mouse'.  The routine '_install_33h' is
;* called by the C startup code if a mouse driver is present.
;******************************************************************************
        NAME    cq_mouse

;******************************************************************************
;*   Include files
;******************************************************************************
        INCLUDE extender.ah
        INCLUDE stk.ah

        PUBLIC  _install_33h            ;Installs 'int33h' intercept routine

        EXTRN   _dos_getvect:NEAR, _dos_setvect:NEAR

;******************************************************************************
;*   Local data
;******************************************************************************
DATA            SEGMENT para public use32 'DATA'

hi_handler      DD      0               ;User's protected-mode mouse handler
                                        ; (high-memory)
low_bridge      dd      0               ;Mouse bridge flat address (low memory)
old_int33h      DD      0               ;Previous interrupt 33h address
old_int48h      DD      0               ;Previous interrupt 48h address

EXTRN   _gda:DWORD

DATA    ENDS

CODE            SEGMENT para public use32 'CODE'
                ASSUME cs:CODE, ds:DATA, es:DATA

mouse_bridge:
;******************************************************************************
;* mouse_bridge
;*
;* This code acts as a "bridge" between the real-mode mouse driver and the
;* user's protected mode handler.  It's only purpose is to allow the user's
;* handler to be called from the mouse driver.  This is done by installing
;* this real-mode routine in low memory, and passing its address to the
;* driver instead of the user's handler address.  This routine then performs
;* a software interrupt (48h).  This interrupt is processed by a protected
;* mode intercept routine installed to call the user's mouse handler (the
;* address of which was previously saved).
;*
;* IMPORTANT note:  This code is assembled in 32-bit mode because it is not
;*    possible to combine 16- & 32-bit segments.  But since the code must
;*    execute in real (16-bit) mode, every instruction must be carefully
;*    examined to make sure the operator & operand are being properly
;*    addressed.  Prefix overrides may have to be specified.  Any changes
;*    made to this code must be verified to be certain the code is as
;*    desired when in 16-bit mode.
;******************************************************************************
ADSZ    EQU     67h                     ;Address size override
OPSZ    EQU     66h                     ;Operand size override


;******************************************************************************
;* 'mb_code' (entry point)
;******************************************************************************
mb_code PROC FAR
MB_CODE_OFF     EQU     $-mouse_bridge  ;Offset to start of code (after data)
                                        ;(Note:  Currently NO data defined)

;******************************************************************************
;* Call user's protected mode mouse handler located in high memory
;******************************************************************************
        int     48h                     ;Call the user's mouse handler

;******************************************************************************
;* Return to mouse driver
;******************************************************************************
        ret

mb_code ENDP

BRIDGE_SIZE     EQU     $-mouse_bridge

;******************************************************************************
;* int33h
;*
;* Interrupt 33h intercept
;*
;* This routine intercepts int 33h and performs processing for the following
;* function calls:
;*    33H, 09H - Set mouse pointer shape (graphics mode).
;*    33H, 0CH - Set user mouse-event-handler.  On return, EAX="MERR" if the
;*                    user's handler was not installed due to lack of low
;*                    memory.
;*
;* This routine could easily be implemented in C using Code Builder.  It is
;* written in assembly language simply to maintain all the mouse code within
;* one source file.  (The preceding routine, 'mouse_bridge', must be 16-bit
;* assembly.)
;*
;* Input:
;*    ebp     - Address of STK image to be sent to the Virtual 86 interrupt
;*                 handler.
;*    ebx     - Address of STK image showing status at time of interrupt.
;*                 If the interrupted process is a Virtual 86 process, ebx
;*                 will be the same as ebp.
;*
;* Output:
;*    All registers must be preserved.
;******************************************************************************
int33h  PROC    NEAR
        pushad

        test    [ebp].STK_FLG,_FLAG_VM  ;Is interrupt from V86 (real) mode?
        jnz     exit_int33h             ;If so, ignore and exit

        mov     eax,[ebp].STK_EAX       ;Retrieve requested interrupt 33h
                                        ; function number
        cmp     al,9                    ;Check for function 9
        jne     SHORT not_3309          ; (set graphics pointer shape)

;******************************************************************************
;* 3309:  Set mouse pointer shape (graphics mode) - Copy shape tbl to low mem
;******************************************************************************
        mov     eax,[ebp].STK_TMP       ;Get gda address, keep in eax
        mov     edi,[eax].GDA_VWRK      ;Set dest addr for copy to 'vwrk' area
        mov     esi,[ebp].STK_EDX       ;Set src addr to user's edx as input
        mov     ecx,64                  ;Length is always fixed at 64 bytes
        cld
    REP movsb

;******************************************************************************
;* Update regs being sent to V86 handler to point to low mem area just copied
;******************************************************************************
        mov     edx,[eax].GDA_VWRK6     ;Get seg:off form of 'vwrk' area addr
        mov     WORD PTR [ebp].STK_EDX,dx       ;Store offset portion
        shr     edx,16                  ;Store segment portion
        mov     [ebp].STK_ES,edx

not_3309:
;******************************************************************************
;* 330C:  Set user-defined mouse event handler
;******************************************************************************
        cmp     al,0ch                  ;Check if function 0C
        jne     SHORT not_330ch         ; (set user mouse event handler)

        call    process_330ch           ;Perform processing for function 0C

not_330ch:

;******************************************************************************
;* Exit 'int33h'
;******************************************************************************
exit_int33h:
        popad
        ret
int33h  ENDP


;******************************************************************************
;* process_330ch
;*
;* This routine is called to process a request to install a user's mouse
;* handler.  Since the handler runs in protected-mode, it cannot be called
;* directly by the real-mode mouse driver.  This routine installs a real-mode
;* "bridge" program in low memory which is capable of calling the user's
;* protected-mode mouse handler.  The address of this "bridge" code is then
;* passed on to the V86 330ch interrupt, thereby installing it as the "user's
;* mouse handler".  When a mouse event occurs which causes the "bridge" code
;* to be executed, it in turn calls the actual user's (protected-mode)
;* handler.
;*
;* If a user handler was previously installed, the V86 interrupt must still
;* be called so that the new handler flags are set.  (The handler address
;* remains the same as before [the "bridge code"].)  The address of the
;* user's handler will be updated to reflect the new handler.
;*
;* Input:
;*    ebp     - Address of STK image to be sent to the Virtual 86 interrupt
;*                 handler.
;*    ebx     - Address of STK image showing status at time of interrupt.
;*    STK_EDX - Address of user's high-memory mouse handler.
;*
;* Output:
;*    No registers preserved.
;*    hi_handler      - Address of user's mouse handler (from 'STK_EDX').
;*    [ebx].STK_EAX   - Set to "MERR" if the user's handler could not be
;*                      installed due to lack of low memory.
;******************************************************************************
process_330ch   PROC    NEAR

        mov     ecx,ebx                 ;Save pointer to interrupted
                                        ; stack frame
;******************************************************************************
;* See if a previous handler was already installed - if so, don't alloc mem
;******************************************************************************
        mov     edx,[ebp].STK_EDX       ;Get user's mouse handler addr
        xchg    edx,hi_handler          ;Store addr of user's high-mem handler
        or      edx,edx                 ;Already installed a user's handler?
        je      SHORT install_mouse_handler ;If not, need to copy mouse bridge

        mov     eax,low_bridge          ;Get addr of prev copied mouse driver
        jmp     SHORT done_new_bridge   ;Need to pass new flags to mouse driver

install_mouse_handler:
;******************************************************************************
;* Allocate low mem needed for "bridge" code to call high-mem handler
;******************************************************************************
        mov     eax,4800h               ;Allocate (low) memory (ah=48h)
        mov     ebx,BRIDGE_SIZE+0fh     ;# of BYTES to alloc (+f to round UP)
        int     21h                     ;Returns alloc'd mem addr in eax

        jnc     SHORT ok_mouse_alloc    ;Allocated memory successfully
        mov     ds:hi_handler,0         ;Unable to alloc mem - can't install
        mov     [ecx].STK_EAX,"MERR"    ;Set user's EAX to "MERR" to show this
        or      [ebp].STK_OPTS,_STK_NOINT ;No need to call DOS (V86) int handlr
        jmp     SHORT done_330ch        ;Give up

ok_mouse_alloc:
        mov     low_bridge,eax          ;Save flat addr of low bridge code

;******************************************************************************
;* Copy "bridge" code to low mem area just allocated
;******************************************************************************
        mov     edi,eax                 ;Base addr of alloc'd low mem is dest.
        MOV     esi,OFFSET mouse_bridge ;Start from top of routine (data area)
        mov     ecx,BRIDGE_SIZE         ;# of bytes to move
        cld
   REP  movsb                           ;Copy "bridge" code to low mem

done_new_bridge:
;******************************************************************************
;* Change addr of user handler in regs going to V86 int handler to "bridge"
;******************************************************************************
        mov     ebx,eax                 ;Copy addr of low mem just alloc'd
        shr     ebx,4                   ;Change to seg:0 (always para bndry)
        mov     [ebp].STK_ES,ebx        ;Store seg of low mem for "bridge" code
        mov     [ebp].STK_EDX,MB_CODE_OFF ;Offset to mouse bridge code

;******************************************************************************
;* Exit 'process_330ch'
;******************************************************************************
done_330ch:
        ret
process_330ch   ENDP

;******************************************************************************
;* _install_33h
;*
;* Installs 'int33h' as the current intercept routine for interrupt 33h.
;* Installs 'int48h' as the current intercept routine for interrupt 48h.
;* Saves addresses of previous intercepts for restore at program termination.
;* Registers 'deinstall_33h' for execution upon program termination
;* to remove 'int33h' and 'int48h'
;*
;* Output:
;*    old_int33h - Address of previous 33h intercept.
;*    old_int48h - Address of previous 48h intercept.
;******************************************************************************
_install_33h    PROC    NEAR
        pushad

        push    33h                     ;Int # of intercept vector desired
        call    _dos_getvect            ;Returns current intercept addr in eax
        add     esp,4                   ;Get rid of arg just pushed
        mov     old_int33h,eax          ;Save existing intercept addr, if any

        push    OFFSET int33h           ;Address of intercept routine
        push    80000033h               ;Int 33h from protected mode, not V86
        call    _dos_setvect            ;Call service routine to set int addr
        add     esp,4*2                 ;Get rid of args just pushed

        push    48h                     ;Int # of intercept vector desired
        call    _dos_getvect            ;Returns current intercept addr in eax
        add     esp,4                   ;Get rid of arg just pushed
        mov     old_int48h,eax          ;Save existing intercept addr, if any

        push    OFFSET int48h           ;Address of intercept routine
        push    48h                     ;Int 48h; from V86 mode, not protected
        call    _dos_setvect            ;Call service routine to set int addr
        add     esp,4*2                 ;Get rid of args just pushed

        mov     edx, _gda               ;Get gda pointer
        mov     eax, OFFSET deinstall_33h ;Address of int33h remove routine
        mov     [edx].GDA_MOUSE, eax    ;Save address of routine for termination

        popad
        ret
_install_33h    ENDP

;******************************************************************************
;* deinstall_33h
;*
;* Removes int 33h & 48h intercepts by replacing them with the previous
;* intercepts.  Called at program termination time by '_cleanup_mouse'.
;*
;* Input:
;*    old_int33h - Address of previous 33h intercept.
;*    old_int48h - Address of previous 48h intercept.
;******************************************************************************
deinstall_33h   PROC    NEAR
        pushad

        push    old_int33h              ;Set address of old int 33h intercept
        push    33h                     ;Interrupt # to set
        call    _dos_setvect            ;Call service routine to set int addr
        add     esp,4*2                 ;Get rid of args just pushed

        push    old_int48h              ;Set address of old int 48h intercept
        push    48h                     ;Interrupt # to set
        call    _dos_setvect            ;Call service routine to set int addr
        add     esp,4*2                 ;Get rid of args just pushed
ok_deinstall_48h:

        popad
        ret

deinstall_33h   ENDP

;******************************************************************************
;* int48h
;*
;* Int 48h intercept
;*
;* This intercept is used as a means for the real mode 'mouse_bridge' code
;* to call a protected mode user's handler.  The 'mouse_bridge' code issues
;* an "int 48h", causing this protected mode intercept to be invoked.
;*
;* It is installed by the C startup code if a mouse driver is present.
;*
;* Input:
;*    ebp     - Address of STK image showing status at time of interrupt.
;*
;* Output:
;*    All registers must be preserved.
;******************************************************************************
int48h  PROC    NEAR

        pushad                          ;Save all registers

;******************************************************************************
;* Call user's handler (ebp still points to STK [_XSTACK] frame)
;******************************************************************************
        call    [hi_handler]            ;Call previously installed user handler

;******************************************************************************
;* Exit 'int48h'
;******************************************************************************
        popad                           ;Restore all registers
;;;;;   or      [ebp].STK_OPTS,STK_NOINT ;Don't call DOS (V86) int handler

        ret

int48h  ENDP

CODE    ENDS

        end

