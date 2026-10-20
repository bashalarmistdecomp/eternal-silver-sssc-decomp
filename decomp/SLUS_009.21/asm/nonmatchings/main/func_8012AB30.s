.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_8012AB30, 0x28

glabel func_8012AB30
    .incbin "expected/code.bin", 0x11AA30, 0x28
endlabel func_8012AB30
