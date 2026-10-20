.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching OpenEvent, 0xC

glabel OpenEvent
    .incbin "expected/code.bin", 0x119F60, 0xC
endlabel OpenEvent
    .incbin "expected/code.bin", 0x119F6C, 0x4
