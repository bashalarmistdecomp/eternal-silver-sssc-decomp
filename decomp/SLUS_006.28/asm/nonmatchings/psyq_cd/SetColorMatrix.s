.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* Handwritten function */
nonmatching SetColorMatrix, 0x30

glabel SetColorMatrix
    .incbin "expected/code.bin", 0x7EF28, 0x30
endlabel SetColorMatrix
