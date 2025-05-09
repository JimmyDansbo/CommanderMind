.import popa			; For retrieveing parameters from C

.export _breakpoint
.export _dbg8
.export _dbg16
.export _vload
.export _enamouse
.export _getmouse
.export _rndcircle
.export _returntobasic
.export _bload
.export _initzsm
.export _zsmplay
.export _zsmstop
.export _zsm_setbank
.export _zsm_setmem
.export _zsm_rewind
.export _zsm_setloop

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

RAM_BANK	= $00
ROM_BANK	= $01

ZSM_INIT_ENGINE = $A000
ZSM_SETISR	= $A054
ZSM_SETBANK	= $A01B
ZSM_SETMEM	= $A01E
ZSM_PLAY	= $A006
ZSM_STOP	= $A009
ZSM_REWIND	= $A00C
ZSM_SETLOOP	= $A033

RED_CIRCLE	= 1
YELLOW_CIRCLE	= 5
BLUE_CIRCLE	= 9
GREEN_CIRCLE	= 13
PURPLE_CIRCLE	= 17
CYAN_CIRCLE	= 21

; *****************************************************************************
; Tell the ZSM kit to start playing music
; *****************************************************************************
_zsmplay:
	tax
	lda	#1
	sta	RAM_BANK
	jmp	ZSM_PLAY

; *****************************************************************************
; Tell the ZSM kit to stop playing music
; *****************************************************************************
_zsmstop:
	tax
	lda	#1
	sta	RAM_BANK
	jmp	ZSM_STOP

; *****************************************************************************
; Initialize the ZSM kit
; *****************************************************************************
_initzsm:
	ldx	#<$0400			; Memory used for ZSM trampoline ($0400)		
	ldy	#>$0400
	lda	#1			; Goto RAM BANK 1 for ZSM 
	sta	RAM_BANK
	jsr	ZSM_INIT_ENGINE
	jmp	ZSM_SETISR

_zsm_setbank:
	pha				; Save priority bank on stack	
	lda	#1			; Ensure correct RAM bank is selected before call
	sta	RAM_BANK
	jsr	popa			; Get priority ID into X
	tax
	pla				; Restore priority bank from stack
	jmp	ZSM_SETBANK

; PER-PRIORITY: set address <- do this second
_zsm_setmem:
	pha				; Save lo part of address on stack
	lda	#1			; Ensure correct RAM bank is selected before call
	sta	RAM_BANK
	txa				; Move hi part of address into .Y
	tay
	jsr	popa			; Get priority ID into X
	tax
	pla				; Restore lo part of address on stack
	jmp	ZSM_SETMEM

; PER-PRIORITY: reset to start of music
_zsm_rewind:
	tax				; Move priority to X
	lda	#1			; Ensure correct RAM bank is selected before call
	sta	RAM_BANK
	jmp	ZSM_REWIND

; PER-PRIORITY: set or clear loop flag
_zsm_setloop:
	pha				; Save loop setting on stack
	lda	#1			; Ensure correct RAM bank is selected before call
	sta	RAM_BANK
	jsr	popa			; Get priority ID into X
	tax
	pla				; Restore loop setting from stack and shift into Carryflag
	lsr
	jmp	ZSM_SETLOOP

; *****************************************************************************
; Use the SMC to reset the system
; *****************************************************************************
_returntobasic:
	ldx	#$42	; System Management Controller
	ldy	#$02	; magic location for system reset
	lda	#$00 	; magic value for system poweroff/reset
	jmp	$FEC9	; reset the computer

; *****************************************************************************
; Find a random number between 1 and 6 and convert it to the index of a sprite
; Essentially returning a random sprite index
; *****************************************************************************
_rndcircle:
	jsr	$FECF	; entropy_get
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
	tay
	lda	spr_indx,y
	rts
spr_indx: .byte 0,RED_CIRCLE,YELLOW_CIRCLE,BLUE_CIRCLE,GREEN_CIRCLE,PURPLE_CIRCLE,CYAN_CIRCLE

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
	ldx	#40		; Screen resolution 40x30
	ldy	#30
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

; *****************************************************************************
; Load a headerless binary file into RAM at specified bank and address
; *****************************************************************************
_bload:
        pha                     ; Save RAM bank on stack

        jsr     popa            ; Get address and store it on stack
        pha
        jsr     popa
        pha

        lda     #1              ; File number, must be unique
        ldx     #8              ; Device 8, local filesystem or SD card
        ldy     #2              ; Secondary command 2 = headerless load
        jsr     $FFBA           ; SETLFS

        jsr     popa            ; Get and save low part of address to filename
        sta     bbase
        jsr     popa            ; Get and save high part of address to filenem
        sta     bbase+1
        ldx     #$FF            ; Find length of filename by searching for 0
:       inx
        lda     $FFFF,x         ; $FFFF will be replaced by address of string
bbase:=*-2
        bne     :-
        txa                     ; Length of filename in A
        ldx     bbase           ; Address of filename
        ldy     bbase+1
        jsr     $FFBD           ; SETNAM

        ply                     ; Pull address to load to from stack
        plx
        pla			; Get RAM bank and set it
	sta	RAM_BANK
	lda	#0              ; 0=load, 1=verify, 2=VRAM,0xxxx, 3=VRAM,1xxxx
        jsr     $FFD5           ; LOAD

        ; 8bit return value must be returned as 16 bit so X and A zeroed
        ldx     #0
        lda     #0
        ; Move carry bit into A
        rol
        ; Invert bit to make it compatible with C true/false
        eor     #1
        rts
