.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching CastlePadStep, 0xAE4

glabel CastlePadStep
    .incbin "expected/code.bin", 0x10D2F0, 0xAE4
endlabel CastlePadStep
