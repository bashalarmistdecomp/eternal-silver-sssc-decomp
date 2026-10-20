.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* Handwritten function */
nonmatching DecDCTvlcSize2, 0x30

glabel DecDCTvlcSize2
    .incbin "expected/code.bin", 0x119910, 0x30
endlabel DecDCTvlcSize2
