.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching prnt, 0x694

glabel prnt
    .incbin "expected/code.bin", 0x11FF00, 0x694
endlabel prnt
    .incbin "expected/code.bin", 0x120594, 0xC
