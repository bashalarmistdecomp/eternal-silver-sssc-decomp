.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* Handwritten function */
nonmatching SetGeomOffset, 0x18

glabel SetGeomOffset
    .incbin "expected/code.bin", 0x7EF78, 0x18
endlabel SetGeomOffset
    .incbin "expected/code.bin", 0x7EF90, 0x8
