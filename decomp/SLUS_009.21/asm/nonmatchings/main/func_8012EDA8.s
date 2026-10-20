.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_8012EDA8, 0x110

glabel func_8012EDA8
    .incbin "expected/code.bin", 0x11ECA8, 0x110
endlabel func_8012EDA8
