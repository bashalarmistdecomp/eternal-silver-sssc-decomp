.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching SpuMalloc, 0x2C4

glabel SpuMalloc
    .incbin "expected/code.bin", 0x11AB50, 0x2C4
endlabel SpuMalloc
    .incbin "expected/code.bin", 0x11AE14, 0xC
