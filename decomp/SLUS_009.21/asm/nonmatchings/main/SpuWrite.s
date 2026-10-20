.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching SpuWrite, 0x5C

glabel SpuWrite
    .incbin "expected/code.bin", 0x11B650, 0x5C
endlabel SpuWrite
    .incbin "expected/code.bin", 0x11B6AC, 0x4
