.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_8012E490, 0xAC

glabel func_8012E490
    .incbin "expected/code.bin", 0x11E390, 0xAC
endlabel func_8012E490
