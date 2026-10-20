.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_801274AC, 0xA4

glabel func_801274AC
    .incbin "expected/code.bin", 0x1173AC, 0xA4
endlabel func_801274AC
