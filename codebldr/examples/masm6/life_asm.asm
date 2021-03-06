        .386
;******************************************************************************
;* Intel 386/486 C Code Builder(TM) Kit
;* Copyright 1991 Intel Corporation.  All Rights Reserved.
;******************************************************************************
;*   LIFE_ASM.ASM
;* 
;*   MASM 6.0 Version
;* 
;*   This file, together with file LIFE.C, illustrates the C-ASM interface.
;*   Contained herein are 2 C-callable routines.  Also, 3 C routines are
;*   called by the Assembly routines in this file.
;*
;*   Refer to the file LIFE.C for details on how the game of Life works, and
;*   for more processing details.
;*
;*   General discussion:
;*      Be certain to use the ".386" directive (usually at the top of your
;*      program).
;*
;*      If you use simplified memory model directives, use ".MODEL small,c".  
;*
;*   Calling C functions from Assembly:
;*      Define the function to be called using the PROTO directive.  If you
;*      use "langtype" 'C', the C code should name the function with a leading
;*      underscore ("_MakeCmpBuf"), but Assembly language references should 
;*      be without one ("MakeCmpBuf").  Using "langtype" 'C' ensures that 
;*      matching INVOKEs push arguments in the order expected by the C routine,
;*      as well as perform stack cleanup after the call.
;*
;*      You cannot use "langtype" 'C' when calling functions defined in the
;*      standard C library, since these functions are not named with a leading
;*      underscore.  Use "langtype" 'SYSCALL' on a PROTO for INVOKEing a
;*      standard C library function.  This will push the arguments in the order
;*      expected by C, but will not perform stack cleanup.  You must perform
;*      stack cleanup to remove the arguments pushed (if any) by the INVOKE.
;*
;*      Your C-callable Assembly procedure must perserve ebp, ebx, esi, and 
;*      edi.  For MASM 6.0, simply add "USES ebx esi edi" as shown in the
;*      examples below.  (The ebp register is automatically preserved by the
;*      the PROC directive.)
;*
;*   Calling Assembly functions from C:
;*      The Assembly FUNC statement should use a 'C' "langtype", listing any
;*      arguments in the same order as invoked from C.  The name must be given
;*      without an added leading underscore ("Life(...)").  When calling from
;*      C, be certain to add an underscore ("_Life(...)").
;*      
;*   Directive "langtype"      C      Assembly  Stack Cleanup
;*   ====================    =====    ========  =============
;*   C                       _Name    Name      Yes
;*   SYSCALL                 Name     Name      No
;*      
;*   
;******************************************************************************
        .MODEL small,c

;******************************************************************************
;*   External variables
;******************************************************************************
        EXTERN  numCols:DWORD           ;# of cols and rows in this text mode
        EXTERN  numRows:DWORD

;******************************************************************************
;*   Function prototypes (external)
;******************************************************************************
getch   PROTO   SYSCALL                 ;C library function
kbhit   PROTO   SYSCALL                 ;C library function
MakeCmpBuf      PROTO C cmpBuf:PTR      ;in LIFE.C

;******************************************************************************
;*   Function prototypes (internal)
;******************************************************************************
CountNeighbors  PROTO C col:DWORD,row:DWORD,cmpBuf:PTR BYTE

;******************************************************************************
;*   VIDEO structure definition (character code and attribute byte)
;******************************************************************************
VIDEO   STRUCT
char    BYTE    ?
attrib  BYTE    ?
VIDEO   ENDS

;******************************************************************************
;*   Equates
;******************************************************************************
PHYS_VID EQU    0b8000h                 ;color monitor, text mode, only


        .CODE


;******************************************************************************
;*   Life(int color, VIDEO *curBuf, VIDEO *prevBuf, BYTE *cmpBuf)
;*
;*   This routine is called by C to play one game of Life.  The first genera-
;*   tion has been created in 'curBuf' and has been displayed on the screen.
;*
;*   Input:
;*      color   - Attribute byte used to determine if a cell is alive or empty.
;*      curBuf  - Pointer to buffer (screen-image-size) which contains copy
;*                      of current screen image.
;*      prevBuf - Pointer to buffer which will be used to hold copy of previous
;*                      screen image (to determine end of game).
;*      cmpBuf  - Pointer to buffer used in counting neighbors of a given cell.
;*                      The buffer is sized to contain only 1 byte per cell,
;*                      and includes 2 extra rows & columns around the peri-
;*                      meter of the actual screen size.  (Used for wrapping.)
;*
;*   Input (global):
;*      numCols, numRows - # of columns & rows in the current text mode.
;*
;*   Returns:
;*      eax - Quit indicator.  0=Do not quit (continue game), 1=quit desired.
;******************************************************************************
Life    PROC    C PUBLIC USES ebx esi edi, color:DWORD, curBuf:PTR VIDEO, 
                prevBuf:PTR VIDEO, cmpBuf:PTR BYTE

        LOCAL   done:BYTE, bufSize:DWORD, curCol:DWORD, curRow:DWORD
        LOCAL   lastCol:DWORD, lastRow:DWORD


;******************************************************************************
;*   Initialize local variables
;******************************************************************************
        mov     eax,numCols             ;set last col # (1 less than max)
        dec     eax
        mov     lastCol,eax             

        mov     eax,numRows             ;set last row #
        dec     eax
        mov     lastRow,eax             

        mov     eax,numCols             ;save screen buf image sz (in WORDS)
        mul     numRows
        mov     bufSize,eax             

;******************************************************************************
;*   Top of loop executed once for each new generation (screen image)
;******************************************************************************
        .REPEAT
                
        mov     done,1                  ;will reset if cell dies or is born
        INVOKE  MakeCmpBuf, cmpBuf      ;call C proc to create comparison buf
                
        xor     eax,eax                 ;init current col#, row# cntrs to 0
        mov     curCol,eax
        mov     curRow,eax

        mov     ebx,curBuf              ;init cur ptr into curBuf (keep in ebx)

newCell:
;******************************************************************************
;*   Top of loop executed once for each cell of the screen image
;*      For each cell, count # of living neighboring cells & determine if cell
;*      should die (or be born if no cell is empty)
;******************************************************************************
        INVOKE  CountNeighbors, curCol,curRow,cmpBuf
        cmp     eax,3                   ;more than 3 neighbors?
        jg      killIt                  ;kill it if it wasn't already dead
        cmp     VIDEO.attrib[ebx],0     ;is this cell currently "dead"?
        jz      isDead                  ;if so, see if it will be reborn or not
        cmp     eax,2                   ;cell alive; if 2 or 3 neighbors, 
        jge     nxtCell                 ;       cell stays alive
killIt:
        mov     VIDEO.attrib[ebx],0     ;doesn't have 2 or 3 neighbors - dies
        mov     done,0                  ;reset done flag, something changed
        jmp     nxtCell

isDead:
        cmp     eax,3                   ;EXACTLY 3 neighbors?
        jne     nxtCell                 ;if not, cell remains dead
        mov     al,BYTE PTR color
        mov     VIDEO.attrib[ebx],al    ;cell is re-born!
        mov     done,0                  ;reset done flag, something changed

nxtCell:
        add     ebx,SIZEOF VIDEO        ;inc to next buf location
        mov     eax,curCol              ;inc to next col, same row
        inc     eax
        mov     curCol,eax
        cmp     eax,lastCol             ;done with this row?
        jle     newCell                 ;if not, try next cell in this row

        mov     curCol,0                ;init col back to 0
        mov     eax,curRow              ;inc to next row
        inc     eax
        mov     curRow,eax
        cmp     eax,lastRow             ;done with this generation (screen)?
        jle     newCell                 ;if not, try first cell in new row

;******************************************************************************
;*   Processed each cell - See if generation is same as 2 generations ago
;******************************************************************************
        mov     esi,curBuf              ;addr of new generation just created
        mov     edi,prevBuf             ;addr of image of previous generation
        mov     ecx,bufSize             ;# of bytes in a screen image
        cld
   REPE cmpsw                           ;loop till difference found or done
        .IF     ZERO?
        mov     done,1                  ;this generation is same as last - done
        .ELSE
        
;******************************************************************************
;*   Not same - Copy current screen image (prev generation) to prevBuf
;******************************************************************************
        mov     esi,PHYS_VID            ;save "new" previous (on screen now)
        mov     edi,PrevBuf             ;copy into buffer which holds prev gen
        mov     ecx,bufSize             ;# of bytes in a screen image
   REP  movsw                           ;copy prev generation to holding buffer
        
        .ENDIF

;******************************************************************************
;*   Update screen image
;******************************************************************************
        mov     esi,curBuf              ;addr of current (new) image
        mov     edi,PHYS_VID            ;update screen image
        mov     ecx,bufSize             ;# of bytes in a screen image
   REP  movsw

;******************************************************************************
;*   See if done (key hit or no changes this generation or repeating gens)
;******************************************************************************
        INVOKE  kbhit                   ;ck for key hit - call C lib function
        .UNTIL  ((eax != 0) || (done != 0))     ;eax as returned by 'kbhit()'
        
;******************************************************************************
;*   Done - clean up keystroke (if one) & exit
;******************************************************************************
        .IF     eax != 0                ;get rid of key hit
        INVOKE  getch
        .IF     eax == 0                ;call again if extended key code
        INVOKE  getch
        .ENDIF
        .ENDIF

        ret
Life    ENDP


;******************************************************************************
;*   CountNeighbors(int col, int row, BYTE *cmpBuf)
;*
;*   This routine determines the number of live neighbor cells of the given
;*   cell at ('row', 'col').  A cell has neighbors as follows:
;*      row-1,col-1     row-1,col       row-1,col+1
;*      row,  col-1     **cell**        row,  col+1
;*      row+1,col-1     row+1,col       row+1,col+1
;*
;*   The count is accomplished by beginning at (row-1,col-1) & progressing
;*   left-to-right, beginning again at the left side for each new row. 
;*
;*   (Remember that 'col' & 'row' when used to index into 'cmpBuf' must be
;*   incremented by 1 due to the extra rows and columns in 'cmpBuf'.)
;*
;*   The buffer 'cmpBuf' is used to determine the cell's neighbors.  The buffer
;*   is created with extra rows and columns around the perimeter of the screen
;*   area.  This means indexing must be done by row+1 and column+1 values.
;*   This allows "automatic" wrapping for references to neighboring cells.
;*
;*   Input:
;*      row,col - Target cell's coordinates.  Must be inc'd by 1 to index into
;*                'cmpBuf' (to skip extra leading row and column).
;*      cmpBuf  - Pointer to buffer used in counting neighbors of a given cell.
;*                The buffer is sized to contain only 1 byte per cell, and
;*                includes 2 extra rows & columns around the perimeter of the 
;*                actual screen size.  (Used for wrapping.)
;******************************************************************************
CountNeighbors  PROC    C USES ebx col:DWORD,row:DWORD,cmpBuf:PTR BYTE

        LOCAL   count:DWORD
        LOCAL   cellAddr:DWORD, cellVal:BYTE

;******************************************************************************
;*   Init local variables & calc target cell's (row-1,col-1) address
;******************************************************************************
        mov     count,0                 ;init neighbor count to 0
        
        mov     edi,numCols             ;# of bytes in one row - keep in edi
        inc     edi                     ;(cmpBuf has 2 extra "missing" cols)
        inc     edi

        mov     eax,row                 ;calc addr of target cell's row-1,col-1
        mul     edi                     ;row# * bytes per row
        add     eax,col                 ;+ col# = offset to row-1,col-1 cell
        add     eax,cmpBuf              ;now have address of target cell
        mov     ebx,eax                 ;keep in ebx

;******************************************************************************
;*   Loop to count each of the neighboring cells
;*   Reg use:  ebx=cmpBuf address   edi=# of bytes in 1 row
;*             ecx=loop counter
;******************************************************************************
        mov     ecx,9
nextCount:
        cmp     ecx,5                   ;is this the target cell?
        je      okCount                 ;don't include in count if so

        cmp     BYTE PTR[ebx],0         ;if neighboring cell is alive, inc #
        je      okCount
        inc     count
okCount:
        
        dec     ecx                     ;dec loop ctr; done if 0
        jz      doneCount
        inc     ebx                     ;inc to next neighbor's addr
        cmp     ecx,3
        je      newRow                  ;count of 3 or 6 means time for new row
        cmp     ecx,6
        jne     nextCount               ;else continue with current row

newRow:
        add     ebx,edi                 ;advance to next row
        sub     ebx,3                   ;dec back to leftmost neighbor in row
        jmp     nextCount

doneCount:
;******************************************************************************
;*   Exit - return neighbor count
;******************************************************************************
        mov     eax,count               ;return # of neighbors of target cell
        ret

CountNeighbors  ENDP



;******************************************************************************
;*   Delay(msecs)
;*
;*   Uses BIOS int 15h, func 8600h to delay for specified # of milli-seconds.
;*
;*   Input:
;*      msecs  -  # of milli-seconds of delay (max 4,294,967 = 4294 secs).
;******************************************************************************
Delay   PROC C PUBLIC USES ebx esi edi, msecs:DWORD

        mov     eax,msecs               ;time must be given in microseconds
        mov     ebx,1000                ;convert from milli- to micro- secs
        mul     ebx
        mov     edx,eax                 ;put result in cx:dx for BIOS call
        and     edx,0ffffh              ;isolate lsw in edx
        mov     ecx,eax
        shr     ecx,16                  ;move msw from hi to lo word of ecx
        mov     eax,8600h               ;func 8600h = delay for specified time
        int     15h

        ret

Delay   ENDP

        END
