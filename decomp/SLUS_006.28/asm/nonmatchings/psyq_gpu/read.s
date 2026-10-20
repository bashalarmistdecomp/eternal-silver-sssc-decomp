.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching read, 0xC

glabel read
    .incbin "expected/code.bin", 0x7A3F8, 0xC
endlabel read
    .incbin "expected/code.bin", 0x7A404, 0x4
