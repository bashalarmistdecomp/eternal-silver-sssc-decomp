.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching _param, 0x30

glabel _param
    .incbin "expected/code.bin", 0x113380, 0x30
endlabel _param
