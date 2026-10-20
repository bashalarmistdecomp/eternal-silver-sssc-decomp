.include "macro.inc"

.section .data, "wa"

/* D_80011060 @ 0x80011060, 100 bytes: text the icon code reads. The bytes are your image's
   (expected/code.bin), as for all of the program's data; the register row
   says what they hold. */

.align 2

nonmatching D_80011060

dlabel D_80011060
    .incbin "expected/code.bin", 0x60, 0x64
enddlabel D_80011060
