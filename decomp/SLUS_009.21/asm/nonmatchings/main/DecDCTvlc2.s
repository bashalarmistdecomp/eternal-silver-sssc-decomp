.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* Handwritten function */
nonmatching DecDCTvlc2, 0x34C

glabel DecDCTvlc2
    .incbin "expected/code.bin", 0x119940, 0x34C
endlabel DecDCTvlc2
    .incbin "expected/code.bin", 0x119C8C, 0x4
