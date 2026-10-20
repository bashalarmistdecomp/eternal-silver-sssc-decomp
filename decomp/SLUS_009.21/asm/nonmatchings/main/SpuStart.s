.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching SpuStart, 0x70

glabel SpuStart
    .incbin "expected/code.bin", 0x119EE8, 0x70
endlabel SpuStart
    .incbin "expected/code.bin", 0x119F58, 0x8
