.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching SpuInitMalloc, 0x54

glabel SpuInitMalloc
    .incbin "expected/code.bin", 0x11AAF0, 0x54
endlabel SpuInitMalloc
    .incbin "expected/code.bin", 0x11AB44, 0xC
