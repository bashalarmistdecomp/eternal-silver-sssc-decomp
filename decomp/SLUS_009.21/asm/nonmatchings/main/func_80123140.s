.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_80123140, 0x280

glabel func_80123140
    .incbin "expected/code.bin", 0x113040, 0x280
endlabel func_80123140
