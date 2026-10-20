.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_8012D134, 0xF0

glabel func_8012D134
    .incbin "expected/code.bin", 0x11D034, 0xF0
endlabel func_8012D134
