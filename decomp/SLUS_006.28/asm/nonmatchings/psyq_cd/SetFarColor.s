.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* Handwritten function */
nonmatching SetFarColor, 0x20

glabel SetFarColor
    .incbin "expected/code.bin", 0x7EF58, 0x20
endlabel SetFarColor
