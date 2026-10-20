.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching _padSioRW2, 0x278

glabel _padSioRW2
    .incbin "expected/code.bin", 0x11D334, 0x278
endlabel _padSioRW2
