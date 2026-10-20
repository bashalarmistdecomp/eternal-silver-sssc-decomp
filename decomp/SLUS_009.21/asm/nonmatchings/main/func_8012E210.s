.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_8012E210, 0x54

glabel func_8012E210
    .incbin "expected/code.bin", 0x11E110, 0x54
endlabel func_8012E210
