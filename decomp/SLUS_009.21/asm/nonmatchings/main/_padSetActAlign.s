.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching _padSetActAlign, 0x68

glabel _padSetActAlign
    .incbin "expected/code.bin", 0x11DF2C, 0x68
endlabel _padSetActAlign
