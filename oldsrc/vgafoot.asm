.386
.MODEL small
DATA SEGMENT USE32
CODE SEGMENT USE32
ASSUME DS:DATA
ASSUME CS:CODE

mode	= 8	;; The requested mode
buff	=16	;; Where to put it

PUBLIC VESA_optinfo
VESA_optinfo PROC NEAR
	enter	0,0
;; Get a buffer in low mem.
	mov	eax, 0		;Clean eax
	mov	ah, 48h		;A buffer por favor!
	mov	bx, 16		;Get 16*16 bytes
	int	21h
	jnc	Cont		;Continue if no error
	mov	eax, 0		;Oops, error.
	leave			;Clean up and return.
	ret

Cont:	push	eax		;Save the adr. of the buffer
	mov	es,  ax
	mov	edi, 0		;??
	mov	ecx, mode[ebp]
	mov	eax, 4F01h
	int	10h


Copy:	mov	ecx, 256	;Copy data
	mov	edi, buff[ebp]
	pop	esi
	rep movsb

Exit:	mov	eax, 49h	;Free mem.
	int	21h
	leave
	ret
VESA_optinfo ENDP
END
