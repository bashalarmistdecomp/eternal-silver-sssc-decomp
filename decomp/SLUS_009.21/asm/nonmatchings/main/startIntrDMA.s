.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching startIntrDMA, 0x4C

glabel startIntrDMA
    .incbin "expected/code.bin", 0x10FAB0, 0x4C
endlabel startIntrDMA
