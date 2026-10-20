.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching strcmp, 0x64

glabel strcmp
    .incbin "expected/code.bin", 0x11FD90, 0x64
endlabel strcmp
    .incbin "expected/code.bin", 0x11FDF4, 0xC
