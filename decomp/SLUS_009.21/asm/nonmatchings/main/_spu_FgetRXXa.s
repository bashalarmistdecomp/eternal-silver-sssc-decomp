.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching _spu_FgetRXXa, 0x3C

glabel _spu_FgetRXXa
    .incbin "expected/code.bin", 0x11A974, 0x3C
endlabel _spu_FgetRXXa
