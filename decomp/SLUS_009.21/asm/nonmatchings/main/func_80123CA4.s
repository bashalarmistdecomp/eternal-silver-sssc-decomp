.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_80123CA4, 0x144

glabel func_80123CA4
    .incbin "expected/code.bin", 0x113BA4, 0x144
endlabel func_80123CA4
