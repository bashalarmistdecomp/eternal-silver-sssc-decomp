.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_801217A4, 0x11C

glabel func_801217A4
    .incbin "expected/code.bin", 0x1116A4, 0x11C
endlabel func_801217A4
