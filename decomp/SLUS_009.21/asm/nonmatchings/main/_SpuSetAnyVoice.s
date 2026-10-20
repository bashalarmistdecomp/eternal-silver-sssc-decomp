.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching _SpuSetAnyVoice, 0x2BC

glabel _SpuSetAnyVoice
    .incbin "expected/code.bin", 0x11B150, 0x2BC
endlabel _SpuSetAnyVoice
    .incbin "expected/code.bin", 0x11B40C, 0x4
