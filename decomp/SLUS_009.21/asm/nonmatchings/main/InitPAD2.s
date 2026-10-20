.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching InitPAD2, 0xC

glabel InitPAD2
    .incbin "expected/code.bin", 0x110100, 0xC
endlabel InitPAD2
    .incbin "expected/code.bin", 0x11010C, 0x4
