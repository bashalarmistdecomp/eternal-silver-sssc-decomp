.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching ChangeClearRCnt, 0x1C

glabel ChangeClearRCnt
    .incbin "expected/code.bin", 0x10FDF0, 0x1C
endlabel ChangeClearRCnt
