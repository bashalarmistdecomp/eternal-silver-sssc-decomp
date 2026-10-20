.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_80095298

dlabel func_80095298
    .incbin "expected/code.bin", 0x84298, 0x8
enddlabel func_80095298
