.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching _spu_Fw, 0x84

glabel _spu_Fw
    .incbin "expected/code.bin", 0x11A7A4, 0x84
endlabel _spu_Fw
