.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching _Pad1, 0x68

glabel _Pad1
    .incbin "expected/code.bin", 0x110054, 0x68
endlabel _Pad1
