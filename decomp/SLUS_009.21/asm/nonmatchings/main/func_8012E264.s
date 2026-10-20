.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_8012E264, 0x60

glabel func_8012E264
    .incbin "expected/code.bin", 0x11E164, 0x60
endlabel func_8012E264
