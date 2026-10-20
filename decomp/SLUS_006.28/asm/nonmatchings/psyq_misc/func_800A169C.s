.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_800A169C, 0x2C

glabel func_800A169C
    .incbin "expected/code.bin", 0x9069C, 0x2C
endlabel func_800A169C
