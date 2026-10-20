.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching DisablePAD, 0x14

glabel DisablePAD
    .incbin "expected/code.bin", 0x110174, 0x14
endlabel DisablePAD
