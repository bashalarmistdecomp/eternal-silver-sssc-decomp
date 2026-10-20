.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching _spu_init, 0x280

glabel _spu_init
    .incbin "expected/code.bin", 0x119F80, 0x280
endlabel _spu_init
