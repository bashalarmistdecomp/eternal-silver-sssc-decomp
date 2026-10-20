.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching LoadTPage, 0xE8

glabel LoadTPage
    .incbin "expected/code.bin", 0x114940, 0xE8
endlabel LoadTPage
    .incbin "expected/code.bin", 0x114A28, 0x8
