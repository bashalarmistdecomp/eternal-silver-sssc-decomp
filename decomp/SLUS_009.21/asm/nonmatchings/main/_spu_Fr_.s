.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching _spu_Fr_, 0xA8

glabel _spu_Fr_
    .incbin "expected/code.bin", 0x11A47C, 0xA8
endlabel _spu_Fr_
