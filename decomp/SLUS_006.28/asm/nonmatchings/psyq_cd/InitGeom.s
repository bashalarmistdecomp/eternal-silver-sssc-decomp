.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* Handwritten function */
nonmatching InitGeom, 0x80

glabel InitGeom
    .incbin "expected/code.bin", 0x7EE10, 0x80
endlabel InitGeom
    .incbin "expected/code.bin", 0x7EE90, 0x8
