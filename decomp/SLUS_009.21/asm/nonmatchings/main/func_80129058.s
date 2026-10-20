.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_80129058, 0x1A8

glabel func_80129058
    .incbin "expected/code.bin", 0x118F58, 0x1A8
endlabel func_80129058
