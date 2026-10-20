.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching open, 0xC

glabel open
    .incbin "expected/code.bin", 0x7A3D8, 0xC
endlabel open
    .incbin "expected/code.bin", 0x7A3E4, 0x4
