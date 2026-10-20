.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_80127BD8, 0x1E8

glabel func_80127BD8
    .incbin "expected/code.bin", 0x117AD8, 0x1E8
endlabel func_80127BD8
