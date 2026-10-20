.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching FlushCache, 0xC

glabel FlushCache
    .incbin "expected/code.bin", 0x7A398, 0xC
endlabel FlushCache
    .incbin "expected/code.bin", 0x7A3A4, 0x4
