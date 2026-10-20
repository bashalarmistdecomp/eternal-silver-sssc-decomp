.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_801229F0, 0x20

glabel func_801229F0
    .incbin "expected/code.bin", 0x1128F0, 0x20
endlabel func_801229F0
