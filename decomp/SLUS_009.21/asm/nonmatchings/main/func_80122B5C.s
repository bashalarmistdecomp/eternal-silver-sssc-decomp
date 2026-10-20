.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_80122B5C, 0x80

glabel func_80122B5C
    .incbin "expected/code.bin", 0x112A5C, 0x80
endlabel func_80122B5C
