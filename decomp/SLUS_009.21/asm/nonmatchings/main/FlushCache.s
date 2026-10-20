.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching FlushCache, 0xC

glabel FlushCache
    .incbin "expected/code.bin", 0x110200, 0xC
endlabel FlushCache
    .incbin "expected/code.bin", 0x11020C, 0x4
