.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching StSetStream, 0x84

glabel StSetStream
    .incbin "expected/code.bin", 0x1183B0, 0x84
endlabel StSetStream
    .incbin "expected/code.bin", 0x118434, 0xC
