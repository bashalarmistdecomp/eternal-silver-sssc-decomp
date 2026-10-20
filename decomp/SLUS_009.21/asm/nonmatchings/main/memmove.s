.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching memmove, 0x6C

glabel memmove
    .incbin "expected/code.bin", 0x121150, 0x6C
endlabel memmove
    .incbin "expected/code.bin", 0x1211BC, 0x4
