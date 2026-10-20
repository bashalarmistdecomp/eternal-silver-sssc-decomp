.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching SpuSetCommonCDVolume, 0x18

glabel SpuSetCommonCDVolume
    .incbin "expected/code.bin", 0x11C380, 0x18
endlabel SpuSetCommonCDVolume
    .incbin "expected/code.bin", 0x11C398, 0x8
