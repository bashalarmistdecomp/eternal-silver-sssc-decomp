.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_80122BDC, 0x18

glabel func_80122BDC
    .incbin "expected/code.bin", 0x112ADC, 0x18
endlabel func_80122BDC
