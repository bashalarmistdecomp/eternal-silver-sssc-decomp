.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_80129978, 0x18

glabel func_80129978
    .incbin "expected/code.bin", 0x119878, 0x18
endlabel func_80129978
