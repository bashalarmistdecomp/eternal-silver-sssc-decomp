.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching ChangeClearPAD, 0xC

glabel ChangeClearPAD
    .incbin "expected/code.bin", 0x83BB8, 0xC
endlabel ChangeClearPAD
    .incbin "expected/code.bin", 0x83BC4, 0x4
