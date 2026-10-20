.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching _spu_Fr, 0x64

glabel _spu_Fr
    .incbin "expected/code.bin", 0x11A828, 0x64
endlabel _spu_Fr
