.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching srand, 0xC

glabel srand
    .incbin "expected/code.bin", 0x11FD80, 0xC
endlabel srand
    .incbin "expected/code.bin", 0x11FD8C, 0x4
