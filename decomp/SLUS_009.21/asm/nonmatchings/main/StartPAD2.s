.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching StartPAD2, 0xC

glabel StartPAD2
    .incbin "expected/code.bin", 0x110110, 0xC
endlabel StartPAD2
    .incbin "expected/code.bin", 0x11011C, 0x4
