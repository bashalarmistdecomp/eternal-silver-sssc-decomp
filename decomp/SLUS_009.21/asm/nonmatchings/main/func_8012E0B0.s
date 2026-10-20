.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_8012E0B0, 0xC8

glabel func_8012E0B0
    .incbin "expected/code.bin", 0x11DFB0, 0xC8
endlabel func_8012E0B0
