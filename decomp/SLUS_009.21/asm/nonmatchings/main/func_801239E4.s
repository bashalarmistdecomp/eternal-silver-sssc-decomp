.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_801239E4, 0x150

glabel func_801239E4
    .incbin "expected/code.bin", 0x1138E4, 0x150
endlabel func_801239E4
