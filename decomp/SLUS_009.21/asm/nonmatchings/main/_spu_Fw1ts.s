.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching _spu_Fw1ts, 0x5C

glabel _spu_Fw1ts
    .incbin "expected/code.bin", 0x11AA58, 0x5C
endlabel _spu_Fw1ts
    .incbin "expected/code.bin", 0x11AAB4, 0xC
