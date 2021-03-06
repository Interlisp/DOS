.386P
DATA SEGMENT USE32
	EXTRN DisplayWidth8: NEAR  ;; DisplayWidth in bytes
DATA ENDS

CODE SEGMENT USE32
ASSUME DS:DATA
ASSUME CS:CODE


;; **************************************************
;; **************************************************
;;                  D o s b b t 1
;; arg1: buffer (pointer to array of word)
;; arg2: left   (dword) (in pixels)
;; arg3: top    (dword) (in pixels)
;; arg4: swidth (dword) (in pixels)
;; arg5: height (dword) (in pixels)
;;
;; Bitblits the image stored in buffer to the display
;; buffer. Assumption: buffer and the displaybuffer
;; are equaly large and thus left, top etc. pertains
;; to the same offsets.
;;
;; PLANAR or PACKED ???
;; 1 bit to 1 bit ???
;;
;; C-CALL: void xDosbbt1( ... );
;; RETURN: 0 in eax. should be ignored...
;; **************************************************
buffer	EQU	 8
left	EQU	12
top	EQU	16
swidth	EQU	20
height	EQU	24

left32	EQU	 -8
width32	EQU	-16

PUBLIC xDosbbt1
xDosbbt1	PROC NEAR
	;;; *****************************
	;;; Save the volatile environment
	;;; *****************************
	enter	16,0

	;;; ******************************
	;;; Adjust width and left to be on even DWORD boundrys.
	;;; Right boundry adjusted to even Dword boundrys (since
	;;; we dma move dwords) and expressed in byte adresses
	;;; (since the arguments are in pixel adresses)
	;;; Same reasoning goes for the left boundry.
	;;; Caveat! Since the DMA transfer is done in dword
	;;; chunks the width has to be expressed in dwords
	;;; ******************************
	;; Right boundry
	mov	eax, left[ebp]
	add	eax, swidth[ebp]
        add     eax, 1fh
	sar	eax, 5		; Get rid of the low 5 bits
	sal	eax, 2		; Adjust up to byte adresses
	push	eax
	;; Left boundry
	mov	eax, left[ebp]
	sar	eax, 5		; Get rid of the low 5 bits
	sal	eax, 2		; Adjust up to byte adresses
	mov	DWORD PTR left32[ebp], eax
	;; width32 = TOS ( = right32 ) - left32
	pop	ebx
	sub	ebx, eax
	sar	ebx, 2
	mov	DWORD PTR width32[ebp], ebx
	;;; ******************************


	;;; ******************************
	;;; Calculate the offset into the buffers
	;;; The buffers are assumed to have the
	;;; offset in common.
	;;; ******************************
	mov	eax, DWORD PTR DisplayWidth8
	mov	ebx, top[ebp]
	imul	eax, ebx
	add	eax, left32[ebp]

	;;; Preamble to banktransfer loop
	mov	esi, buffer[ebp]; src into ESI
	add	esi, eax	; adjust for offset
	mov	edi, 0a0000h	; dest into EDI
	add	edi, eax	; adjust for offset
	mov	ecx, height[ebp]; line counter

	;; push the results
 nuln:	push	ecx	; push the column counter
	push	esi
	push	edi
	mov	ecx, width32[ebp]	; swidth into ecx
	and	ecx, 0ffffh	; adjust ecx to the bank width

	;; write one line to the display bank
   @50: ;; The actual byte swapping
	lodsd           ; Load eax and increment esi
        xchg    ah,al   ; Swap low byte
        rol     eax,10h ; Get the high byte into position
        xchg    ah,al   ; Swap again
        not     eax     ; In medley 1=black 0=white, Hence invert.
	stosd           ; Store eax and increment edi
        loop    @50

	;; Get the src and dest for the start of the line.
	pop	edi
	pop	esi

	;; calc vals for src and dest for the next line.
	add	edi, DWORD PTR DisplayWidth8	; dest = edi + displaywidth
	add	esi, DWORD PTR DisplayWidth8	; src  = esi + swidth

	pop	ecx	;; get the row counter
	loop	nuln

	;;; *****************************
	;;; Provide return value
	;;; *****************************
	mov	eax,0

	;;; *****************************
	;;; Restore the stack and return
	;;; *****************************
	leave
        ret
xDosbbt1 ENDP

CODE ENDS
END
