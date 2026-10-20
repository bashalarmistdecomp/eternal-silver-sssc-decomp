.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching _padLoadActInfo, 0xD4

glabel _padLoadActInfo
    .incbin "expected/code.bin", 0x11D9B8, 0xD4
endlabel _padLoadActInfo
