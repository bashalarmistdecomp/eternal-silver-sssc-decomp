.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_801233F8, 0x40

glabel func_801233F8
    .incbin "expected/code.bin", 0x1132F8, 0x40
endlabel func_801233F8
