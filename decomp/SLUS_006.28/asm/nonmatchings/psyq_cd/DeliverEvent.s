.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching DeliverEvent, 0xC

glabel DeliverEvent
    .incbin "expected/code.bin", 0x7F448, 0xC
endlabel DeliverEvent
    .incbin "expected/code.bin", 0x7F454, 0x4
