.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_80123B34, 0x13C

glabel func_80123B34
    .incbin "expected/code.bin", 0x113A34, 0x13C
endlabel func_80123B34
