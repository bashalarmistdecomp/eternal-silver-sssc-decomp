.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_80129734, 0x90

glabel func_80129734
    .incbin "expected/code.bin", 0x119634, 0x90
endlabel func_80129734
