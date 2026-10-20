.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_801234D4, 0x2B0

glabel func_801234D4
    .incbin "expected/code.bin", 0x1133D4, 0x2B0
endlabel func_801234D4
