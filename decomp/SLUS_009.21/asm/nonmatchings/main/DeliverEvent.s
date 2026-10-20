.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching DeliverEvent, 0xC

glabel DeliverEvent
    .incbin "expected/code.bin", 0x115060, 0xC
endlabel DeliverEvent
    .incbin "expected/code.bin", 0x11506C, 0x4
