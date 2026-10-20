.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_80122AA8, 0x98

glabel func_80122AA8
    .incbin "expected/code.bin", 0x1129A8, 0x98
endlabel func_80122AA8
