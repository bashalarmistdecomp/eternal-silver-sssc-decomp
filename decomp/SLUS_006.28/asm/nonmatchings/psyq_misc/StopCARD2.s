.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching StopCARD2, 0xC

glabel StopCARD2
    .incbin "expected/code.bin", 0x90648, 0xC
endlabel StopCARD2
    .incbin "expected/code.bin", 0x90654, 0x4
