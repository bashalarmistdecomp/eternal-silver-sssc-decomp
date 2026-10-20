.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching _padSioRW, 0x210

glabel _padSioRW
    .incbin "expected/code.bin", 0x11D124, 0x210
endlabel _padSioRW
