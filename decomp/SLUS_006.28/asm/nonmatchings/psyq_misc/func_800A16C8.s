.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_800A16C8, 0x44

glabel func_800A16C8
    .incbin "expected/code.bin", 0x906C8, 0x44
endlabel func_800A16C8
