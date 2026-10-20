.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_80127860, 0x34

glabel func_80127860
    .incbin "expected/code.bin", 0x117760, 0x34
endlabel func_80127860
