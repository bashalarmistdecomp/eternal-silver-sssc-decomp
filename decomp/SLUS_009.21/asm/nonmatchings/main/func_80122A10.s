.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_80122A10, 0x98

glabel func_80122A10
    .incbin "expected/code.bin", 0x112910, 0x98
endlabel func_80122A10
