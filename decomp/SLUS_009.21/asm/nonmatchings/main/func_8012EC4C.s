.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_8012EC4C, 0x118

glabel func_8012EC4C
    .incbin "expected/code.bin", 0x11EB4C, 0x118
endlabel func_8012EC4C
