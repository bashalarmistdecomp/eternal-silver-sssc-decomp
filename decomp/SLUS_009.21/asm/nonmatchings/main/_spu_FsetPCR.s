.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching _spu_FsetPCR, 0x58

glabel _spu_FsetPCR
    .incbin "expected/code.bin", 0x11A9B0, 0x58
endlabel _spu_FsetPCR
