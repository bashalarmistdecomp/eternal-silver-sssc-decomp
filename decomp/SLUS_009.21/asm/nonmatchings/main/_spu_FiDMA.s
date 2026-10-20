.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching _spu_FiDMA, 0xBC

glabel _spu_FiDMA
    .incbin "expected/code.bin", 0x11A3C0, 0xBC
endlabel _spu_FiDMA
