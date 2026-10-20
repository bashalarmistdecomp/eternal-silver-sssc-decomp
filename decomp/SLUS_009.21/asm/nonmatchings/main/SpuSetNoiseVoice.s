.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching SpuSetNoiseVoice, 0x24

glabel SpuSetNoiseVoice
    .incbin "expected/code.bin", 0x11B120, 0x24
endlabel SpuSetNoiseVoice
    .incbin "expected/code.bin", 0x11B144, 0xC
