.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching LoadClut2, 0x64

glabel LoadClut2
    .incbin "expected/code.bin", 0x114A30, 0x64
endlabel LoadClut2
    .incbin "expected/code.bin", 0x114A94, 0xC
