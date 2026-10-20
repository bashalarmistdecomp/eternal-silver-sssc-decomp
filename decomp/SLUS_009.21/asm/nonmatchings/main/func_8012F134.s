.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_8012F134, 0x7C

glabel func_8012F134
    .incbin "expected/code.bin", 0x11F034, 0x7C
endlabel func_8012F134
