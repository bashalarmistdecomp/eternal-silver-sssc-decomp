.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching kanjitrans, 0x52C

glabel kanjitrans
    .incbin "expected/code.bin", 0x1142DC, 0x52C
endlabel kanjitrans
