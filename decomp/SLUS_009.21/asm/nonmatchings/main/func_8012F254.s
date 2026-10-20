.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_8012F254, 0x270

glabel func_8012F254
    .incbin "expected/code.bin", 0x11F154, 0x270
endlabel func_8012F254
