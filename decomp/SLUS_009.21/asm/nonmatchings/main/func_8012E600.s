.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_8012E600, 0x394

glabel func_8012E600
    .incbin "expected/code.bin", 0x11E500, 0x394
endlabel func_8012E600
    .incbin "expected/code.bin", 0x11E894, 0xC
