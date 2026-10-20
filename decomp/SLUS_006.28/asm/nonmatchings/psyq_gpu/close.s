.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching close, 0xC

glabel close
    .incbin "expected/code.bin", 0x7A418, 0xC
endlabel close
    .incbin "expected/code.bin", 0x7A424, 0x4
