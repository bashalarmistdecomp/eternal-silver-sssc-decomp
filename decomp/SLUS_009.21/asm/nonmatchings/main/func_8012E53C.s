.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_8012E53C, 0xC4

glabel func_8012E53C
    .incbin "expected/code.bin", 0x11E43C, 0xC4
endlabel func_8012E53C
