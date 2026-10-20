.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching TestEvent, 0xC

glabel TestEvent
    .incbin "expected/code.bin", 0x11B7F0, 0xC
endlabel TestEvent
    .incbin "expected/code.bin", 0x11B7FC, 0x4
