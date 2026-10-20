.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_8012FBF8, 0xAC

glabel func_8012FBF8
    .incbin "expected/code.bin", 0x11FAF8, 0xAC
endlabel func_8012FBF8
    .incbin "expected/code.bin", 0x11FBA4, 0xC
