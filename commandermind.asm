.import popa			; For retrieveing parameters from C

.export _breakpoint
.export _dbg8
.export _dbg16

.segment "CODE"

; *****************************************************************************
; Introduce a breakpoint into the emulators debugger
; *****************************************************************************
_breakpoint:
_dbg8:
_dbg16:
        .byte $db
        rts
