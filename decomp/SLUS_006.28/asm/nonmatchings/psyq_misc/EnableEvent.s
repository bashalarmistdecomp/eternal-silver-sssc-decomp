.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching EnableEvent, 0xC

glabel EnableEvent
    .incbin "expected/code.bin", 0x8CE68, 0xC
endlabel EnableEvent
    .incbin "expected/code.bin", 0x8CE74, 0x4
