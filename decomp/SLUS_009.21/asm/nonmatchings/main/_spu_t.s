.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching _spu_t, 0x280

glabel _spu_t
    .incbin "expected/code.bin", 0x11A524, 0x280
endlabel _spu_t
