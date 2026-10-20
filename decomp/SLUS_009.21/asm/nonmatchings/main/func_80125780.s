.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_80125780, 0x564

glabel func_80125780
    .incbin "expected/code.bin", 0x115680, 0x564
endlabel func_80125780
