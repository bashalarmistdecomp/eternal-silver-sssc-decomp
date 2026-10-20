.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching PAD_dr, 0xC

glabel PAD_dr
    .incbin "expected/code.bin", 0x10FD80, 0xC
endlabel PAD_dr
    .incbin "expected/code.bin", 0x10FD8C, 0x4
