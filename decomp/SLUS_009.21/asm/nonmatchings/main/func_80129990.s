.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_80129990, 0x7C

glabel func_80129990
    .incbin "expected/code.bin", 0x119890, 0x7C
endlabel func_80129990
    .incbin "expected/code.bin", 0x11990C, 0x4
