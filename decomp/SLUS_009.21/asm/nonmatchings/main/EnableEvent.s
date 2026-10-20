.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching EnableEvent, 0xC

glabel EnableEvent
    .incbin "expected/code.bin", 0x119F70, 0xC
endlabel EnableEvent
    .incbin "expected/code.bin", 0x119F7C, 0x4
