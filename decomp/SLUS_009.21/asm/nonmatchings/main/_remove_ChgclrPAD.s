.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* Handwritten function */
nonmatching _remove_ChgclrPAD, 0x68

glabel _remove_ChgclrPAD
    .incbin "expected/code.bin", 0x110210, 0x68
endlabel _remove_ChgclrPAD
    .incbin "expected/code.bin", 0x110278, 0x8
