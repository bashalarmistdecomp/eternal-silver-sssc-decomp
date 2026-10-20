.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching SpuSetCommonAttr, 0x37C

glabel SpuSetCommonAttr
    .incbin "expected/code.bin", 0x11B800, 0x37C
endlabel SpuSetCommonAttr
    .incbin "expected/code.bin", 0x11BB7C, 0x4
