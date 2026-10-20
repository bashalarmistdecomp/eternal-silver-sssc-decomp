.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_8012A300, 0x1C0

glabel func_8012A300
    .incbin "expected/code.bin", 0x11A200, 0x1C0
endlabel func_8012A300
