.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_801233E4, 0x14

glabel func_801233E4
    .incbin "expected/code.bin", 0x1132E4, 0x14
endlabel func_801233E4
