.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching strlen, 0x34

glabel strlen
    .incbin "expected/code.bin", 0x11FE00, 0x34
endlabel strlen
    .incbin "expected/code.bin", 0x11FE34, 0xC
