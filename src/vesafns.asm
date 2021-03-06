.386P
DATA SEGMENT USE32


;;; ******************************
;;; DspInterfaceRec is the record that represents the
;;; display interface. 
;;;  **IT IS IMPORTANT THAT YOU KEEP THIS RECORD IN SYNC
;;; WITH THE DspInterfaceRec DEFINED IN C. 
;;; ******************************
	DevRec			STRUC
;	active			DD	?
;	locked			DD	?
	deventer		DD	?
	devexit			DD	?
	before_raid		DD	?
	after_raid		DD	?
;	sync_device		DD	?
	DevRec	ENDS

	DspInterfaceRec		STRUC
	device			DevRec	<>
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

;	mouse_invissible	DD	?
;	mouse_vissible		DD	?

	displaywidth		DD	?
	displayheight		DD	?
	bitsperpixel		DD	?
	colors			DD	?
	oldstate		DD	?
	graphicsstate		DD	?
	numberofbanks		DD	?

	BytesPerLine		DD	?
	DisplayStartAddr	DD	?
	DspInterfaceRec ENDS

;;; ******************************
;;; IOPAGE good old iopage from medley...
;;;  **IT IS IMPORTANT THAT YOU KEEP THIS RECORD IN SYNC
;;; WITH THE DspInterfaceRec DEFINED IN C. 
;;; ******************************

	IOPAGE			STRUC
	dummy0				DW 	22o DUP	(?)
        dlfloppycmd			DW			?
	dlmaintpanel			DW			?
        dlprocessorcmd			DW			?
        dlttyportcmd			DW			?
        dlbeepcmd			DW			?
        newmousestate			DW			?
        dlrs232cputflag			DW			?
        dlrs232cmisccommand		DW			?
	dummy1b				DW			?
	dlrs232cgetflag			DW			?
	dummy1				DW	4o DUP		(?)
	dlfloppy			DW			?
	dummy1a				DW			?
	dummy2				DW			?
	dlttyout			DW			?
	dummy3				DW			?
	dlttyin				DW			?
	dlprocessor1			DW			?
	dlprocessor2			DW			?
	newmousex			DW			?
	dlprocessor0			DW			?
	dlbeepfreq			DW			?
	newmousey			DW			?
	dlrs232cparametercsbhi		DW			?
	dlrs232cparametercsblo		DW			?
	dlrs232csetrs366status		DW	2o DUP		(?)
	dlrs232cputcsblo		DW			?
	dlrs232csetrs366statusa		DW			?
	dlrs232cgetcsblo		DW			?
	dlrs232cputcsbhi		DW			?
	dlrs232cdevicestatus		DW			?
	dlrs232cgetcsbhi		DW			?
	dltodvalid			DW			?
	dlrs232cparameteroutcome	DW			?
	dltodhi				DW			?
	dltodlo				DW			?
	dlmousex			DW			?
	dltodlo2			DW			?
	dlutilin			DW			?
	dlmousey			DW			?
	dlkbdad1			DW			?
	dlkbdad0			DW			?
	dlkbdad3			DW			?
	dlkbdad2			DW			?
	dlkbdad5			DW			?
	dlkbdad4			DW			?
	dllsepimagecsb			DW	40o DUP		(?)
	dummy4a				DW			?
	dliophardwareconfig		DW			?
	dummy4				DW	12o DUP		(?)
	dlrs232cparametercsbhi_11	DW			?
	dlrs232cparametercsblo_11	DW			?
	dlrs232csetrs366status_11	DW	16o DUP		(?)
	dummy5				DW	74o DUP		(?)
	dlmagtape			DW	 4o DUP		(?)
	dlethernet			DW	14o DUP		(?)
	dummy6				DW	36o DUP		(?)
	dldispinterrupt			DW			?
	dummy6a				DW			?
	dldispborder			DW			?
	dldispcontrol			DW			?
	dlcursory			DW			?
	dlcursorx			DW			?
	dlcursorbitmap			DW	20o DUP		(?)
	IOPAGE			ENDS

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
	push edx
	push ecx
	push edi
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
	pop edi
	pop ecx
	pop edx
	RET2C	0
Dosclearbanks ENDP



;; **************************************************
;;	 D O S C u r s o r V i s s i b l e
;; arg1: dsp	(pointer to dsp struct)
;; arg2: iop	(pointer to IOPAGE struct
;; Medley's cursor has no meaningfull mask. The mask
;;  is just the invers of the map (sigh...). The function
;;  f(bg, map, curs) = bg*mask + (not mask)*curs thus
;;  collapses to bg*(not curs) + curs. Since the medley
;;  bitmaps have the invers meaning of the vesa bitmaps
;;  (ie. they are pre inverted for your convenience!)
;;  the expression turns out to be:
;;  bg*curs + (not curs)
;;
;; The general idea here is that we blit the cursor
;; directly to the screen instead of to the displayregion.
;; this saves a whole bunch of time since takeing the
;; mouse down is just a matter of updateing the screen.
;; since this operation has to be done every charblt
;; we save bunches of time.	/jarl 
;;
;; Assumption: we can straddle at most two banks
;; **************************************************
dsp		=	 8
iop		=	 12
PUBLIC DOSCursorVissible
DOSCursorVissible	PROC NEAR
	enter	32,0

	push edx
	push esi
	push edi
	push ebx
	push ecx
	mov	edx, dsp[ebp]
	mov	esi, iop[ebp]

	;; find the destiniation byte index
	movzx	eax, [esi.dlcursory]
	imul	eax, [edx.BytesPerLine]
	movzx	ebx, [esi.dlcursorx]
	sar	ebx, 3		; Make it a byte address
	add	eax, ebx
	mov	edi, eax
	;; make the dest index be an address within bounds
	and	edi, 0ffffh
	or	edi, [edx.DisplayStartAddr]

setbnk:	mov	 dx, 3cdh	; Set the bank
	mov	 ax, 0
	out	 dx, ax

	add	esi, dlcursorbitmap
	mov	ecx, 16		; The curs height

bltcur:	lodsw			; cursorbitmap to ax
	mov	ax, 1010101010101010b
	stosw
	add	edi, [edx.BytesPerLine]

	loop	bltcur

	pop ecx
	pop ebx
	pop edi
	pop esi
	pop edx

	RET2C	0
DOSCursorVissible ENDP


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
dsp		= 8	
buffer		=12
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
	push ebx
	push ecx
	push edx
	push esi
	push edi

	mov	edx, dsp[ebp]

	;; Adjust the arguments to fit inside the display
	mov	eax, [edx.displaywidth]
	cmp	left[ebp], eax	; if left > displaywidth
	jg	alldone
	mov	eax, [edx.displayheight]
	cmp	top[ebp], eax	; if top > displayheight
	jg	alldone
	cmp	DWORD PTR left[ebp], 0	; if 0 > left foldwidth
	jle	foldwidth
cmpwdt:	cmp	DWORD PTR top[ebp], 0	; if 0 > top foldheight
	jle	foldheight
cmphgt:	cmp	DWORD PTR swidth[ebp], 0	; if 0 > width
	jle	alldone
	cmp	DWORD PTR height[ebp], 0	;if 0 > height
	jle	alldone
	jmp	startpoint

foldwidth:
	mov	eax, left[ebp]
	add	swidth[ebp], eax
	mov	DWORD PTR left[ebp], 0
	jmp	cmpwdt

foldheight:
	mov	eax, top[ebp]
	add	height[ebp], eax
	mov	DWORD PTR top[ebp], 0
	jmp	cmphgt

	;; Calculate byte offset into bitmap
startpoint:
	mov	eax, [edx.displaywidth]
	imul	eax, top[ebp]
	add	eax, left[ebp]
	sar	eax, 5		; Make it a byte address on dword boundrys.
	sal	eax, 2

	;; Set dst and src start
	mov	edi, eax
	mov	esi, eax
	add	edi, [edx.DisplayStartAddr]
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
	jg	Newline1

alldone:
	pop edi
	pop esi
	pop edx
	pop ecx
	pop ebx

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
;; FUNCTION: Monocrome bbt to a 4-plane displaybuffer.
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
buffer		=	 12
left		=	 16
top		=	 20
swidth		=	 24
height		=	 28

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
	push ebx
	push ecx
	push edx
	push esi
	push edi

	mov	edx, dsp[ebp]

	;; Adjust the arguments to fit inside the display
	mov	eax, [edx.displaywidth]
	cmp	left[ebp], eax	; if left > displaywidth
	jg	allbye
	mov	eax, [edx.displayheight]
	cmp	top[ebp], eax	; if top > displayheight
	jg	allbye
	cmp	DWORD PTR left[ebp], 0	; if 0 > left foldwidth
	jle	foldw
cmpwd:	cmp	DWORD PTR top[ebp], 0	; if 0 > top foldheight
	jle	foldh
cmphg:	cmp	DWORD PTR swidth[ebp], 0	; if 0 > width
	jle	allbye
	cmp	DWORD PTR height[ebp], 0	;if 0 > height
	jle	allbye
	jmp	startpt

foldw:
	mov	eax, left[ebp]
	add	swidth[ebp], eax
	mov	DWORD PTR left[ebp], 0
	jmp	cmpwd

foldh:
	mov	eax, top[ebp]
	add	height[ebp], eax
	mov	DWORD PTR top[ebp], 0
	jmp	cmphg

	;; Calculate byte offset into bitmap
startpt:
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

allbye:
	pop edi
	pop esi
	pop edx
	pop ecx
	pop ebx
	RET2C	0
Dosbbt2 ENDP

;; **************************************************
;; **************************************************
;;                  D o s b b t 3
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
;; Medley has a packed bitmap structure. Dosbbt3 assumes
;; that we are operating in 4-plane mode. The medley
;; bitmap is blitted to the first plane of the display.
;; Thus the bitmap appears black and white.
;;
;;
;; C-CALL: void Dosbbt3( ... );
;; RETURN: 0 in eax. should be ignored...
;; **************************************************
dsp		= 8
buffer		=	 12
left		=	 16
top		=	 20
swidth		=	 24
height		=	 28

srcend		=	 -8

PUBLIC Dosbbt3
Dosbbt3	PROC NEAR
	;;; *****************************
	;;; Save the volatile environment
	;;; *****************************
	enter	32,0
	push ebx
	push ecx
	push edx
	push esi
	push edi

	;;; Set up the dsp in edx
	mov	edx, dsp[ebp]

	;;; Adjust left to be a byte offset at a dword boundry
	;;; - Not needed. We shovle bytes at byte boundrys

	;;; Adjust width to be a byte offset at a dword boundry
	;;; - Not needed. We shovle bytes at byte boundrys

	;;; Calculate start index for src
	mov	eax, top[ebp]
	imul	eax, [edx.BytesPerLine]
	add	eax, left[ebp]
	mov	esi, eax

	;;; Calculate start index  for dst.
	mov	edi, eax

	;;; Calculate end address for src
	mov	eax, top[ebp]
	add	eax, height[ebp]
	imul	eax, [edx.BytesPerLine]
	add	eax, buffer[ebp]
	mov	srcend[ebp], eax

	;;; Calculate the dstincr, ie. what to add to dst to
	;;; get to the next line
	mov	eax, [edx.BytesPerLine]
	sub	eax, swidth[ebp]
	mov	dstincr[ebp], eax

	;;; Calculate the srcincr, ie. what to add to src to
	;;; get to the next line
	mov	srcincr[ebp], eax

	;;; Calculate the start address for the src
	 ;; We already know the byte index. Easy calculat'n
	add	esi, buffer[ebp]	;esi now points to src

Newbank3:
	;; Set the bank
	mov	eax, esi	; 1. Calculate the index.
	sub	eax, buffer[ebp]
	sar	eax, 10h	; 2. Divide by "bytes-per-buffer"
				; WARNING! this implies buffersize.
	mov	dx, 3cdh
	out	dx, ax		; 3. Set the port.

	;; Adjust dst to be within the bank.
	and	edi, 0ffffh
	or	edi, 0a0000h

Newline3:
	mov	ecx, swidth[ebp]	; width into ecx
	rep movsb		; Dump a line to the display

	;; calc vals for src and dest for the next line.
	add	edi, dstincr[ebp]
	add	esi, srcincr[ebp]

	;; End of block?
	cmp	esi, srcend[ebp]
	jge	QuitThis	; Yes, scram.

	;; End of bank ?
	cmp	ax, 0
	jge	Newline3	; No. Blitt a new line.
	mov	edi, esi	; Yes. Reload edi,
	jmp	Newbank3	; and blitt a new line.

QuitThis:
	pop edi
	pop esi
	pop edx
	pop ecx
	pop ebx
	RET2C	0
Dosbbt3 ENDP

CODE ENDS
END
