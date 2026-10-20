.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching CloseEvent, 0xC

glabel CloseEvent
    .incbin "expected/code.bin", 0x931D8, 0xC
endlabel CloseEvent
    .incbin "expected/code.bin", 0x931E4, 0x4
