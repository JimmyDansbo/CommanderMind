.import popa			; For retrieveing parameters from C

.export _breakpoint
.export _dbg8
.export _dbg16
.export _vload
.export _enamouse
.export _getmouse
.export _rndcircle

.segment "CODE"
; ZeroPage variables/pointers
TMP0		= $30
TMP1		= $31
TMP2		= $32
TMP3		= $33
TMP4		= $34
TMP5		= $35
TMP6		= $36
TMP7		= $37
TMP8		= $38
TMP9		= $39
TMPa		= $3A
TMPb		= $3B
TMPc		= $3C
TMPd		= $3D
TMPe		= $3E
TMPf		= $3F
TMP_PTR0	= TMP0
TMP_PTR1	= TMP2
TMP_PTR2	= TMP4
TMP_PTR3	= TMP6
TMP_PTR4	= TMP8
TMP_PTR5	= TMPa
TMP_PTR6	= TMPc
TMP_PTR7	= TMPe

_rndcircle: .byte $db
	jsr	$FECF		; entropy_get
	eor	TMPf
	stx	TMPf
	eor	TMPf
	sty	TMPf
	eor	TMPf
	sta	TMPf
	lsr
	lsr
	lsr
	lsr
	eor	TMPf
	and	#7
	cmp	#0
	beq	_rndcircle
	cmp	#7
	beq	_rndcircle
	; Here we have a number between 1 and 6, both inclusive. Convert it to sprite index
	ldx	#0
	cmp	#6
	bne	:+
	lda	#21	;cyan circle
	rts
:	cmp	#5
	bne	:+
	lda	#17	;purple circle
	rts
:	cmp	#4
	bne	:+
	lda	#13	;green circle
	rts
:	cmp	#3
	bne	:+
	lda	#9
	rts
:	cmp	#2
	bne	:+
	lda	#5
:	rts

; *****************************************************************************
; Get mouse state, coordinates as button presses
; *****************************************************************************
_getmouse:
        tax
        jsr    $FF6B
        rts

; *****************************************************************************
; Enable and show default mouse pointer
; *****************************************************************************
_enamouse:
        sec
        jsr     $FF5F           ; screen_mode
        lda     #1
        jmp     $FF68           ; mouse_config

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

        ply                     ; Pull address to load to from stack
        plx
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
