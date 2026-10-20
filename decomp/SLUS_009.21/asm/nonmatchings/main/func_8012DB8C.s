.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_8012DB8C, 0xA8

glabel func_8012DB8C
    .incbin "expected/code.bin", 0x11DA8C, 0xA8
endlabel func_8012DB8C
