.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_801233C0, 0x24

glabel func_801233C0
    .incbin "expected/code.bin", 0x1132C0, 0x24
endlabel func_801233C0
