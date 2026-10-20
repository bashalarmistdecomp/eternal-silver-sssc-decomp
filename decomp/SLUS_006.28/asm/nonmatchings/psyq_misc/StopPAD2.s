.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching StopPAD2, 0xC

glabel StopPAD2
    .incbin "expected/code.bin", 0x904D8, 0xC
endlabel StopPAD2
    .incbin "expected/code.bin", 0x904E4, 0x4
