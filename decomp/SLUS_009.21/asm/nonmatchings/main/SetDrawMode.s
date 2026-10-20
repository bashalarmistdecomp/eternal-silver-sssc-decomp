.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching SetDrawMode, 0x94

glabel SetDrawMode
    .incbin "expected/code.bin", 0x114D50, 0x94
endlabel SetDrawMode
    .incbin "expected/code.bin", 0x114DE4, 0xC
