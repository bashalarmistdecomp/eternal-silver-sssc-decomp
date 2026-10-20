.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_80123784, 0x260

glabel func_80123784
    .incbin "expected/code.bin", 0x113684, 0x260
endlabel func_80123784
