.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching _bu_init, 0xC

glabel _bu_init
    .incbin "expected/code.bin", 0x7A3A8, 0xC
endlabel _bu_init
    .incbin "expected/code.bin", 0x7A3B4, 0x4
