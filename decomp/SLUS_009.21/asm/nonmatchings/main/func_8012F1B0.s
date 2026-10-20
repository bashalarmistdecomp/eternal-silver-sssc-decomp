.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching func_8012F1B0, 0x70

glabel func_8012F1B0
    .incbin "expected/code.bin", 0x11F0B0, 0x70
endlabel func_8012F1B0
