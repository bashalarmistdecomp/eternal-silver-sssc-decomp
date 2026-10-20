.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching write, 0xC

glabel write
    .incbin "expected/code.bin", 0x1208C0, 0xC
endlabel write
    .incbin "expected/code.bin", 0x1208CC, 0x4
