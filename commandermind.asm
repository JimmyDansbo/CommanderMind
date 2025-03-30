.import popa			; For retrieveing parameters from C

.export _breakpoint
.export _dbg8
.export _dbg16
.export _vload

.segment "CODE"

; *****************************************************************************
; Introduce a breakpoint into the emulators debugger
; *****************************************************************************
_breakpoint:
_dbg8:
_dbg16:
        .byte $db
        rts

; *****************************************************************************
; Load a binary file into VRAM at specified bank and address
; *****************************************************************************
_vload:
        inc                     ; Increment VERA bank number 2 times to make it
        inc                     ; fit with the LOAD call (2=0xxxx, 3=1xxxx)
        pha                     ; Save VERA bank on stack

        jsr     popa            ; Get address and store it on stack
        pha
        jsr     popa
        pha

        lda     #1              ; File number, must be unique
        ldx     #8              ; Device 8, local filesystem or SD card
        ldy     #2              ; Secondary command 2 = headerless load
        jsr     $FFBA           ; SETLFS

        jsr     popa            ; Get and save low part of address to filename
        sta     vbase
        jsr     popa            ; Get and save high part of address to filenem
        sta     vbase+1
        ldx     #$FF            ; Find length of filename by searching for 0
:       inx
        lda     $FFFF,x         ; $FFFF will be replaced by address of string
vbase:=*-2
        bne     :-
        txa                     ; Length of filename in A
        ldx     vbase           ; Address of filename
        ldy     vbase+1
        jsr     $FFBD           ; SETNAM

        plx                     ; Pull address to load to from stack
        ply
        pla                     ; 0=load, 1=verify, 2=VRAM,0xxxx, 3=VRAM,1xxxx
        jsr     $FFD5           ; LOAD

        ; 8bit return value must be returned as 16 bit so X and A zeroed
        ldx     #0
        lda     #0
        ; Move carry bit into A
        rol
        ; Invert bit to make it compatible with C true/false
        eor     #1
        rts
