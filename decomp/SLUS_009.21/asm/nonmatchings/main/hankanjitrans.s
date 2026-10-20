.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching hankanjitrans, 0x134

glabel hankanjitrans
    .incbin "expected/code.bin", 0x114808, 0x134
endlabel hankanjitrans
    .incbin "expected/code.bin", 0x11493C, 0x4
