.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching _spu_gcSPU, 0x300

glabel _spu_gcSPU
    .incbin "expected/code.bin", 0x11AE20, 0x300
endlabel _spu_gcSPU
