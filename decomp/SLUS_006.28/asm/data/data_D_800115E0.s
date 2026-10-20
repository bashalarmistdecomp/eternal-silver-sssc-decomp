.include "macro.inc"

.section .data, "wa"

/* D_800115E0 @ 0x800115E0, 40 bytes: ten words the CD reader reads. The bytes are your image's
   (expected/code.bin), as for all of the program's data; the register row
   says what they hold. */

.align 2

nonmatching D_800115E0

dlabel D_800115E0
    .incbin "expected/code.bin", 0x5E0, 0x28
enddlabel D_800115E0
