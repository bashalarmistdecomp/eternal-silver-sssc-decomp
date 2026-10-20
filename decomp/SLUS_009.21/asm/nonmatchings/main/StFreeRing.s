.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching StFreeRing, 0xAC

glabel StFreeRing
    .incbin "expected/code.bin", 0x118440, 0xAC
endlabel StFreeRing
    .incbin "expected/code.bin", 0x1184EC, 0x4
