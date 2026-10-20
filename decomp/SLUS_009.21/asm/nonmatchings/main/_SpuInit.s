.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching _SpuInit, 0xE8

glabel _SpuInit
    .incbin "expected/code.bin", 0x119E00, 0xE8
endlabel _SpuInit
