.386
.MODEL huge
NAME dosbbt


	DATA SEGMENT
	PUBLIC
	EXTRN DisplayHeight: NEAR
	EXTRN DisplayWidth: NEAR
	ORG 00H
	PUBLIC tmpbuffer
	tmpbuffer               DB 4 DUP(00H)
	DATA ENDS


	CODE SEGMENT
	PUBLIC
       EXTRN _setvideomode: NEAR
       EXTRN _putimage: NEAR
       EXTRN _imagesize: NEAR
       EXTRN _getvideoconfig: NEAR
       EXTRN exit: NEAR
       EXTRN free: NEAR
       EXTRN calloc: NEAR

                        PUBLIC DOSbitbli
ORG 00H

BIT_SRC		EQU	000000014h
SRC_LEFT	EQU	000000018h
SRC_TOP		EQU	00000001ch
SRC_WIDTH	EQU	000000020h
SRC_HEIGHT	EQU	000000024h

DEST_INDEX	EQU	0fffffffch
SRC_RIGHT	EQU	0fffffff4h
SRC_BOTTOM	EQU	0fffffff8h
	; DEST_ROW	EQU
	; SRC_ROW		EQU
	; DEST_COL	EQU
	; SRC_COL		EQU
ROW_LIMIT	EQU	0ffffffe8h
PIXEL		EQU	0ffffffe4h
SRCWIDTH32	EQU	0ffffffe0h
DSPWIDTH32	EQU	0ffffffdch

                                                   ; Statement # 1081
ASSUME DS:DATA
ASSUME CS:CODE
                           DOSbitbli      proc near
                                   push    ebx
                                   push    esi
                                   push    edi
                                   push    ebp
                                   mov     ebp,esp
                                   sub     esp,1ch
                                                   ; Statement # 1094
                                   mov     DWORD PTR [ebp]+DEST_INDEX,0h

                                                   ; Statement # 1101
                                                   ;   source_right = (source_left + source_width + 31)/32
                                   mov     eax,DWORD PTR [ebp]+SRC_WIDTH
                                   mov     ecx,DWORD PTR [ebp]+SRC_LEFT
                                   add     eax,ecx
                                   add     eax,1fh
                                   mov     ebx,20h
                                   cdq
                                   idiv    ebx
                                   mov     DWORD PTR [ebp]+SRC_RIGHT,eax

                                                   ; Statement # 1102
                                                   ; source_bottom = source_top + source_height
                                   mov     edx,DWORD PTR [ebp]+SRC_TOP
                                   mov     ebx,DWORD PTR [ebp]+SRC_HEIGHT
                                   add     edx,ebx
                                   mov     DWORD PTR [ebp]+SRC_BOTTOM,edx


                                                   ; Statement # 1105
                                                   ; source_left /= 32
                                   xchg    eax,ecx
                                   mov     ecx,20h
                                   cdq
                                   idiv    ecx
                                   mov     DWORD PTR [ebp]+SRC_LEFT,eax

                                                   ; Statement # 1107
                                                   ; tmpbuffer[dest_index++] =
                                                   ;        (source_height << 16)
                                                   ;        + ((source_right - source_left) * 32);
                                   mov     edx,DWORD PTR [ebp]+SRC_RIGHT
                                   sub     edx,eax
                                   imul    edx,ecx
                                   sal     ebx,10h
                                   add     edx,ebx
                                   mov     eax,DWORD PTR [ebp]+DEST_INDEX
                                   inc     DWORD PTR [ebp]+DEST_INDEX
                                   mov     ecx,DWORD PTR tmpbuffer
                                   mov     DWORD PTR [ecx][eax*4],edx


                                                   ; Statement # 1113
                                   mov     edi,DWORD PTR [ebp]+SRC_TOP
                                   jmp     @5
                                   nop
                                   nop
                           @3:
                                   inc     edi
                           @5:
                                   cmp     DWORD PTR [ebp]+SRC_BOTTOM,edi
                                   jbe     @4
                                                   ; Statement # 1114



                                                   ; 
                                                   ; 
                                                   ; Head of the redundant loop
                                                   ; 
                                   mov     DWORD PTR [ebp]+0ffffffe4h,0h ; PIXEL = 0
                                   jmp     @8
                                   nop
                           @6:
                                   inc     DWORD PTR [ebp]+PIXEL         ; PIXEL++
                           @8:
                                   cmp     DWORD PTR [ebp]+PIXEL,4h      ; PIXEL < 4
                                   jnb     @3
                                                   ; 
                                                   ;
                                                   ; 
                                                   ; 

                                                   ; Statement # 1117
                                   mov     eax,edi
                                   imul    eax,DWORD PTR DisplayWidth
                                   shr     eax,5h
                                   mov     esi,eax
                                   mov     ecx,DWORD PTR [ebp]+SRC_LEFT
                                   add     esi,ecx
                                   mov     eax,DWORD PTR [ebp]+SRC_RIGHT
                                   add     eax,esi
                                   sub     eax,ecx
                                   mov     DWORD PTR [ebp]+ROW_LIMIT,eax
                                   jmp     @11
                           @9:
                                   inc     esi     ; SOURCE_COL++
                           @11:
                                   cmp     esi,DWORD PTR [ebp]+ROW_LIMIT  ; SOURCE_COL < ROW_LIMIT ?
                                   jnb     @6


                                                   ; Statement # 1118
                                   mov     eax,DWORD PTR [ebp]+BIT_SRC
                                   mov     eax,DWORD PTR [eax][esi*4]
                                                   ; 
                                                   ; 
                                                   ; HACK. Byteswap and invert for speed.
                                                   ; This is done in a tight loop.
                                                   ; 
                                                   ; 
                                                   ; Now source[source_col] is in exa
                                   xchg    ah,al   ; Swap low byte
                                   rol     eax,10h ; Get the high byte into position
                                   xchg    ah,al   ; Swap again
                                   not     eax     ; In medley 1=black 0=white, Hence invert.
                                                   ; 
                                                   ; 

                                   mov     ebx,DWORD PTR [ebp]+DEST_INDEX
                                   inc     DWORD PTR [ebp]+DEST_INDEX
                                   mov     ecx,DWORD PTR tmpbuffer
                                   mov     DWORD PTR [ecx][ebx*4],eax


                                                   ; Statement # 1122
                                   jmp     @9
                                   nop
                                   nop
                                   nop
                                                   ; Statement # 1123
                           @4:
                                                   ; Statement # 1124
                                   push    3h
                                   push    DWORD PTR tmpbuffer; 2
                                   fild    DWORD PTR [ebp]+SRC_TOP
                                   sub     esp,8h
                                   fstp    QWORD PTR [ebp-2ch]
                                   mov     eax,DWORD PTR [ebp]+SRC_LEFT
                                   sal     eax,5h
                                   push    eax     ; 5
                                   fild    DWORD PTR [ebp-30h]
                                   pop     eax     ; 5
                                   sub     esp,8h
                                   fstp    QWORD PTR [ebp-34h]
                                   call    _putimage
                                   add     esp,18h
                                                   ; Statement # 1125
                                   leave
                                   pop     edi
                                   pop     esi
                                   pop     ebx
                                   ret
                           DOSbitbli      endp

	CODE ENDS
.STACK 88
END
