.import popa			; For retrieveing parameters from C

.export _dummy

.segment "CODE"

_dummy: .byte $db
	pha
	pla
	; Return value of carry bit
	lda	#0		; Empty .A
	rol			; Move carry bit to .A
	eor	#1		; Invert value so it works in C code
	ldx	#0		; Zero out X as X=high byte of 16bit return value
	rts
