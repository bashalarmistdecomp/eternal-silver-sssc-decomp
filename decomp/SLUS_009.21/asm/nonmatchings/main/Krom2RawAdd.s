.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching Krom2RawAdd, 0xC

glabel Krom2RawAdd
    .incbin "expected/code.bin", 0x1112B0, 0xC
endlabel Krom2RawAdd
    .incbin "expected/code.bin", 0x1112BC, 0x4
