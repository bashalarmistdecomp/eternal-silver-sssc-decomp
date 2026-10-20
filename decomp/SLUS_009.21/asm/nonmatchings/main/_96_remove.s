.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching _96_remove, 0xC

glabel _96_remove
    .incbin "expected/code.bin", 0x10FD68, 0xC
endlabel _96_remove
    .incbin "expected/code.bin", 0x10FD74, 0xC
