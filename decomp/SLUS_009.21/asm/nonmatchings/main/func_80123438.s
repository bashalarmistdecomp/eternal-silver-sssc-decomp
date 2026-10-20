.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_80123438, 0x48

glabel func_80123438
    .incbin "expected/code.bin", 0x113338, 0x48
endlabel func_80123438
