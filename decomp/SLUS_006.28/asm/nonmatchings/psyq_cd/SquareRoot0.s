.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* Handwritten function */
nonmatching SquareRoot0, 0x84

glabel SquareRoot0
    .incbin "expected/code.bin", 0x7EE98, 0x84
endlabel SquareRoot0
    .incbin "expected/code.bin", 0x7EF1C, 0xC
