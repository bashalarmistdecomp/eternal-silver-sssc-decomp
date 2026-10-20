.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching SpuSetKey, 0x1BC

glabel SpuSetKey
    .incbin "expected/code.bin", 0x11B460, 0x1BC
endlabel SpuSetKey
    .incbin "expected/code.bin", 0x11B61C, 0x4
