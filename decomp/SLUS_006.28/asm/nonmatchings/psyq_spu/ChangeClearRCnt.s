.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching ChangeClearRCnt, 0xC

glabel ChangeClearRCnt
    .incbin "expected/code.bin", 0x83BC8, 0xC
endlabel ChangeClearRCnt
    .incbin "expected/code.bin", 0x83BD4, 0x4
