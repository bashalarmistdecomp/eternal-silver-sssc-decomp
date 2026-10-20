.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching DecDCTvlcBuild, 0xE4

glabel DecDCTvlcBuild
    .incbin "expected/code.bin", 0x119C90, 0xE4
endlabel DecDCTvlcBuild
    .incbin "expected/code.bin", 0x119D74, 0xC
