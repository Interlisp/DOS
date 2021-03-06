.386P
DATA SEGMENT USE32

;;; ******************************
;;; ScreenBitmap mis the record that describes the
;;; physical display (compare X11).
;;;  **IT IS IMPORTANT THAT YOU KEEP THIS RECORD IN SYNC
;;; WITH THE BitmapRec DEFINED IN C. 
;;; ******************************

;;; ******************************
;;; DspInterfaceRec is the record that represents the
;;; display interface. 
;;;  **IT IS IMPORTANT THAT YOU KEEP THIS RECORD IN SYNC
;;; WITH THE DspInterfaceRec DEFINED IN C. 
;;; ******************************
	DspInterfaceRec		STRUC

	enter_display		DD	?
	exit_display		DD	?

	before_raid		DD	?
	after_raid		DD	?

	drawline		DD	?

	cleardisplay		DD	?

	get_color_map_entry	DD	?
	set_color_map_entry	DD	?
	available_colors	DD	?
	possible_colors		DD	?

;;	get_color_map		DD	?
;;	set_color_map		DD	?
;;	make_color_map		DD	?

	medley_to_native_bm	DD	?
	native_to_mdley_bm	DD	?

	bitblit_to_screen	DD	?
	bitblit_from_screen	DD	?
	scroll_region		DD	?

	displaywidth		DD	?
	displayheight		DD	?
	bitsperpixel		DD	?
	colors			DD	?
	oldstate		DD	?
	graphicsstate		DD	?
	numberofbanks		DD	?

	DspInterfaceRec ENDS

	EXTRN DisplayWidth8: NEAR  ;; DisplayWidth in bytes

DATA ENDS

CODE SEGMENT USE32
ASSUME DS:DATA
ASSUME CS:CODE

DOBANK	MACRO	NUMBER
	push	eax
	mov	 ax, NUMBER
	mov	 dx, 3cdh
	out	 dx, ax
	pop	eax
	ENDM

RET2C	MACRO	VALUE
	mov	eax,VALUE
	leave
        ret
	ENDM

OUTBANK	MACRO
	LOCAL	Dumpline, Newline
Newline:
	mov	ecx, width32[ebp]	; swidth into ecx

Dumpline:
	lodsd           ; Load eax and increment esi
        xchg    ah,al   ; Swap low byte
        rol     eax,10h ; Get the high byte into position
        xchg    ah,al   ; Swap again
        not     eax     ; In medley 1=black 0=white, Hence invert.
	stosd           ; Store eax and increment edi
        loop    Dumpline

	;; calc vals for src and dest for the next line.
	add	edi, dstincr[ebp]
	add	esi, srcincr[ebp]

	dec	rowcount[ebp]
	jnz	Newline
	ENDM

;; **************************************************
;;		 D o s c l e a r b a n k s
;; arg1: dsp	(pointer to dsp struct)
;;
;; Fill banks with 0.
;; **************************************************
dsp		=	 8
bank		=	-8
PUBLIC Dosclearbanks
Dosclearbanks	PROC NEAR
	enter	16,0
	mov	DWORD PTR bank[ebp], 0
	mov	edx, dsp[ebp]
	mov	ecx, [edx.numberofbanks]

clrbnk:	push	ecx
	DOBANK	bank[ebp]
	add	DWORD PTR bank[ebp], 1
	mov	eax, 0
	mov	edi, 0a0000h
	mov	ecx, 0ffffh
	rep stosd

	pop	ecx
	loop	clrbnk
	RET2C	0
Dosclearbanks ENDP
;; **************************************************
;; **************************************************
;;                  D o s b b t 1
;; arg1: dsp	(pointer to a dsp struct)
;; arg2: buffer (pointer to array of word)
;; arg3: left   (dword) (in pixels)
;; arg4: top    (dword) (in pixels)
;; arg5: swidth (dword) (in pixels)
;; arg6: height (dword) (in pixels)
;;
;; Bitblits the image stored in buffer to the display
;; buffer. Assumption: buffer and the displaybuffer
;; are equaly large and thus left, top etc. pertains
;; to the same offsets.
;;
;;
;; C-CALL: void Dosbbt1( ... );
;; RETURN: 0 in eax. should be ignored...
;; **************************************************
dsp		=	 8
buffer		=	12
left		=	16
top		=	20
swidth		=	24
height		=	28

left32		=	 -8
width32		=	-12
endptr		=	-16
desttop		=	-20
dstincr		=	-24
srcincr		=	-28


PUBLIC Dosbbt1
Dosbbt1	PROC NEAR
	;;; *****************************
	;;; Save the volatile environment
	;;; *****************************
	enter	32,0

	;; Calculate byte offset into bitmap
	mov	edx, dsp[ebp]
	mov	eax, [edx.displaywidth]
	imul	eax, top[ebp]
	add	eax, left[ebp]
	sar	eax, 5		; Make it a byte address on dword boundrys.
	sal	eax, 2

	;; Set dst and src start
	mov	edi, eax
	mov	esi, eax
	add	edi, 0a0000h
	add	esi, buffer[ebp]

	;; Set dst and src incr
	mov	eax, left[ebp]
	add	eax, swidth[ebp]
        add     eax, 1fh
	sar	eax, 5
	mov	ebx, left[ebp]
	sar	ebx, 5
	sub	eax, ebx
	mov	width32[ebp], eax	; width32 is width in dwords
	sal	eax, 2		; Make width32 a byteadr on dword boundrys.
	mov	ebx, [edx.displaywidth]
	sar	ebx, 3
	sub	ebx, eax

	;; {dst,src}incr is what to add to {esi,edi} to get to the new line
	mov	dstincr[ebp], ebx
	mov	srcincr[ebp], ebx

	;;; ******************************
Newline1:
	mov	ecx, width32[ebp]	; swidth into ecx

Dumpline1:
	lodsd           ; Load eax and increment esi
        xchg    ah,al   ; Swap low byte
        rol     eax,10h ; Get the high byte into position
        xchg    ah,al   ; Swap again
        not     eax     ; In medley 1=black 0=white, Hence invert.
	stosd           ; Store eax and increment edi
        loop    Dumpline1

	;; calc vals for src and dest for the next line.
	add	edi, dstincr[ebp]
	add	esi, srcincr[ebp]

	dec	DWORD PTR height[ebp]
	jnz	Newline1

	RET2C	0
Dosbbt1 ENDP


;; **************************************************
;; **************************************************
;;                  D o s b b t 2
;; arg1: dsp	(pointer to a dsp struct)
;; arg2: buffer (pointer to array of word)
;; arg3: left   (dword) (in pixels)
;; arg4: top    (dword) (in pixels)
;; arg5: swidth (dword) (in pixels)
;; arg6: height (dword) (in pixels)
;;
;; Bitblits the image stored in buffer to the display
;; buffer. ASSUMPTION: buffer and the displaybuffer
;; are equaly large and thus left, top etc. pertains
;; to the same offsets.
;;
;; Medley has a packed bitmap structure. Dosbbt2 assumes
;; that we are operating in 4-plane mode. The medley
;; bitmap is blitted to the first plane of the display.
;; Thus the bitmap appears black and white.
;;
;;
;; C-CALL: void Dosbbt2( ... );
;; RETURN: 0 in eax. should be ignored...
;; **************************************************
dsp		=	 8
buffer		=	12
left		=	16
top		=	20
swidth		=	24
height		=	28

left32		=	 -8
width32		=	-12
endptr		=	-16
desttop		=	-20
dstincr		=	-24
srcincr		=	-28
tmpheight	=	-32


lines_per_bank	= 512

PUBLIC Dosbbt2
Dosbbt2	PROC NEAR
	;;; *****************************
	;;; Save the volatile environment
	;;; *****************************
	enter	32,0

	;; Calculate byte offset into bitmap
	mov	edx, dsp[ebp]
	mov	eax, [edx.displaywidth]
	imul	eax, top[ebp]
	add	eax, left[ebp]
	sar	eax, 5		; Make it a byte address on dword boundrys.
	sal	eax, 2

	;; Calculate which bank to start in.
	mov	DWORD PTR bank[ebp], eax
	sar	DWORD PTR bank[ebp], 10h

	;; Set dst and src start
	mov	edi, eax
	mov	esi, eax
	add	esi, buffer[ebp]

	;; Set dst and src incr
	mov	eax, left[ebp]
	add	eax, swidth[ebp]
        add     eax, 1fh
	sar	eax, 5
	mov	ebx, left[ebp]
	sar	ebx, 5
	sub	eax, ebx
	mov	width32[ebp], eax	; width32 is width in dwords
	sal	eax, 2		; Make width32 a byteadr on dword boundrys.
	mov	ebx, [edx.displaywidth]
	sar	ebx, 3
	sub	ebx, eax

	;; {dst,src}incr is what to add to {esi,edi} to get to the new line
	mov	dstincr[ebp], ebx
	mov	srcincr[ebp], ebx

	;; Adjust top to be inside the startbank
	and	DWORD PTR top[ebp], 1ffh	;lines_per_bank

Newbank2:
	;; Set the bank
	mov	 eax, DWORD PTR bank[ebp]
	mov	 dx, 3cdh
	out	 dx, ax
	;; Adjust dst to be within axxxxh
	and	edi, 0ffffh
	or	edi, 0a0000h

	;; XX
	mov	ebx, height[ebp]
	mov	eax, top[ebp]
	add	eax, ebx
	cmp	eax, lines_per_bank
	jle	doit

	mov	ebx, lines_per_bank
	sub	ebx, top[ebp]

doit:	
	mov	DWORD PTR top[ebp], 0
	mov	tmpheight[ebp], ebx
	sub	height[ebp], ebx

Newline2:
	mov	ecx, width32[ebp]	; swidth into ecx

Dumpline2:
	lodsd           ; Load eax and increment esi
        xchg    ah,al   ; Swap low byte
        rol     eax,10h ; Get the high byte into position
        xchg    ah,al   ; Swap again
        not     eax     ; In medley 1=black 0=white, Hence invert.
	stosd           ; Store eax and increment edi
        loop    Dumpline2

	;; calc vals for src and dest for the next line.
	add	edi, dstincr[ebp]
	add	esi, srcincr[ebp]
	dec	DWORD PTR tmpheight[ebp]
	jg	Newline2

	inc	DWORD PTR bank[ebp]

	cmp	DWORD PTR height[ebp], 0
	jg	Newbank2

	RET2C	0
Dosbbt2 ENDP


CODE ENDS
END
