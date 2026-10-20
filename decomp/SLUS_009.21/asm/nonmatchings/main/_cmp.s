.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching _cmp, 0x20

glabel _cmp
    .incbin "expected/code.bin", 0x1170C8, 0x20
endlabel _cmp
