.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching tolower, 0x34

glabel tolower
    .incbin "expected/code.bin", 0x1205D4, 0x34
endlabel tolower
    .incbin "expected/code.bin", 0x120608, 0x8
