.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_8012E3B8, 0xD8

glabel func_8012E3B8
    .incbin "expected/code.bin", 0x11E2B8, 0xD8
endlabel func_8012E3B8
