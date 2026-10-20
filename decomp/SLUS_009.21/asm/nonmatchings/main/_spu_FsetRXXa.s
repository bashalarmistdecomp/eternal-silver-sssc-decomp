.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching _spu_FsetRXXa, 0xA4

glabel _spu_FsetRXXa
    .incbin "expected/code.bin", 0x11A8D0, 0xA4
endlabel _spu_FsetRXXa
