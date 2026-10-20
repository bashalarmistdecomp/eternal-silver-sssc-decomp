.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_80126E10, 0xD8

glabel func_80126E10
    .incbin "expected/code.bin", 0x116D10, 0xD8
endlabel func_80126E10
    .incbin "expected/code.bin", 0x116DE8, 0x8
