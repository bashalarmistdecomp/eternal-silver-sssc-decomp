.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_80123DE8, 0xA0

glabel func_80123DE8
    .incbin "expected/code.bin", 0x113CE8, 0xA0
endlabel func_80123DE8
