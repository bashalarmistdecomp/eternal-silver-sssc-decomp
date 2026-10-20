.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_801200A4, 0x78

glabel func_801200A4
    .incbin "expected/code.bin", 0x10FFA4, 0x78
endlabel func_801200A4
