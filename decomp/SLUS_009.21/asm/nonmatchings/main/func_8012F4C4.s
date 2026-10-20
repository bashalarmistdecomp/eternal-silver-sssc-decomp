.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_8012F4C4, 0x3E8

glabel func_8012F4C4
    .incbin "expected/code.bin", 0x11F3C4, 0x3E8
endlabel func_8012F4C4
