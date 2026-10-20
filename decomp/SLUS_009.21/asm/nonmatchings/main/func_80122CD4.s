.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_80122CD4, 0x230

glabel func_80122CD4
    .incbin "expected/code.bin", 0x112BD4, 0x230
endlabel func_80122CD4
