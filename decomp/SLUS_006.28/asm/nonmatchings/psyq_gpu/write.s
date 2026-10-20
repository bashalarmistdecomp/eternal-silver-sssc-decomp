.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching write, 0xC

glabel write
    .incbin "expected/code.bin", 0x7A408, 0xC
endlabel write
    .incbin "expected/code.bin", 0x7A414, 0x4
