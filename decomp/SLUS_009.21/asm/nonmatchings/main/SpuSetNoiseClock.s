.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching SpuSetNoiseClock, 0x48

glabel SpuSetNoiseClock
    .incbin "expected/code.bin", 0x11B410, 0x48
endlabel SpuSetNoiseClock
    .incbin "expected/code.bin", 0x11B458, 0x8
