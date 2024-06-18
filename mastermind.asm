!cpu w65c02
!src "../cx16stuff/cx16.inc"
+SYS_LINE
!src "vtuilib-acme.inc"

STARTX = 34
STARTY = 7

!macro DRAWPIN .xcord, .ycord, .col {
	+VTUI_GOTOXY .xcord, .ycord
	+VTUI_PLOT_CHAR $55, .col
	+VTUI_PLOT_CHAR $49, .col
	lda	VERA_ADDR_L
	sec
	sbc	#4
	sta	VERA_ADDR_L
	inc	VERA_ADDR_M
	+VTUI_PLOT_CHAR $4A, .col
	+VTUI_PLOT_CHAR $4B, .col
}
!macro DRAWPIN .xcord, .ycord, .col, .chr {
	+VTUI_GOTOXY .xcord, .ycord
	+VTUI_PLOT_CHAR .chr, ~.col
	+VTUI_PLOT_CHAR .chr, ~.col
	lda	VERA_ADDR_L
	sec
	sbc	#4
	sta	VERA_ADDR_L
	inc	VERA_ADDR_M
	+VTUI_PLOT_CHAR .chr, ~.col
	+VTUI_PLOT_CHAR .chr, ~.col
}

main:
	+VTUI_SCREEN_SET $00
	jsr	initscreen
	rts

initscreen:
	+VTUI_GOTOXY STARTX, STARTY-4
	+VTUI_PRINT_STR heading, ccol-heading, 0, $61	; X16MASTERMIND
	+VTUI_GOTOXY 1, STARTY+4
	+VTUI_PRINT_STR ccol, ccolmn-ccol, 0, $61	; CHOOSE COLOR:
	+DRAWPIN STARTX-33, STARTY+6, colors+0, $A0
	+DRAWPIN STARTX-30, STARTY+6, colors+1, $A0
	+DRAWPIN STARTX-27, STARTY+6, colors+2, $A0
	+DRAWPIN STARTX-24, STARTY+6, colors+3, $A0
	+DRAWPIN STARTX-21, STARTY+6, colors+4, $A0
	+DRAWPIN STARTX-18, STARTY+6, colors+5, $A0
	+DRAWPIN STARTX-15, STARTY+6, colors+6, $A0
	+DRAWPIN STARTX-12, STARTY+6, colors+7, $A0
	+VTUI_GOTOXY STARTX-32, STARTY+9
	+VTUI_PRINT_STR colb, heading-colb, 0, $61	; B  W  R  P  G U  Y  O
	+VTUI_GOTOXY 13, STARTY+15
	+VTUI_BORDER 2, 4, 4, $61
	+VTUI_GOTOXY STARTX-14, STARTY+34
	+VTUI_PRINT_STR ccolmn, end-ccolmn, 0, $61	; CHOOSE COLUMN:
	jsr	drawgrid

drawgrid:
	+VTUI_GOTOXY STARTX, STARTY	; 34, 5
	+VTUI_BORDER 2, 13, 34, $61
	lda	#STARTX+3
	jsr	dovline
	lda	#STARTX+6
	jsr	dovline
	lda	#STARTX+9
	jsr	dovline
	ldy	#STARTY+3
	jsr	dohline
	ldy	#STARTY+6
	jsr	dohline
	ldy	#STARTY+9
	jsr	dohline
	ldy	#STARTY+12
	jsr	dohline
	ldy	#STARTY+15
	jsr	dohline
	ldy	#STARTY+18
	jsr	dohline
	ldy	#STARTY+21
	jsr	dohline
	ldy	#STARTY+24
	jsr	dohline
	ldy	#STARTY+27
	jsr	dohline
	ldy	#STARTY+30
	jsr	dohline
	+DRAWPIN STARTX+1, STARTY+1, colors+8, $A0
	+DRAWPIN STARTX+4, STARTY+1, colors+8, $A0
	+DRAWPIN STARTX+7, STARTY+1, colors+8, $A0
	+DRAWPIN STARTX+10,STARTY+1, colors+8, $A0
	rts

dovline:
	ldy	#STARTY
	jsr	vtui_gotoxy
	+VTUI_PLOT_CHAR $72, $61
	inc	VERA_ADDR_M
	dec	VERA_ADDR_L
	dec	VERA_ADDR_L
	+VTUI_VLINE $5D, 32, $61
	+VTUI_PLOT_CHAR $71, $61
	rts

dohline:
	lda	#STARTX
	jsr	vtui_gotoxy
	+VTUI_PLOT_CHAR $6B, $61
	+VTUI_HLINE $43, 2, $61
	+VTUI_PLOT_CHAR $5B, $61
	+VTUI_HLINE $43, 2, $61
	+VTUI_PLOT_CHAR $5B, $61
	+VTUI_HLINE $43, 2, $61
	+VTUI_PLOT_CHAR $5B, $61
	+VTUI_HLINE $43, 2, $61
	+VTUI_PLOT_CHAR $73, $61
	rts

selcols:!byte	$FF, $FF, $FF, $FF
chosen:	!byte	$FF

colors:	!byte	$60, $61, $62, $64, $65, $6E, $67, $68, $6F
colb:	!pet	"b  w  r  p  g  u  y  o"
heading:!pet	"x16mastermind"
ccol:	!pet	"choose color:"
ccolmn:	!pet	"choose column: 1  2  3  4"
end:
