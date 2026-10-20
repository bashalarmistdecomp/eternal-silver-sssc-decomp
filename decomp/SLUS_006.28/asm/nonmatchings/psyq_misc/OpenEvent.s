.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching OpenEvent, 0xC

glabel OpenEvent
    .incbin "expected/code.bin", 0x8CE58, 0xC
endlabel OpenEvent
    .incbin "expected/code.bin", 0x8CE64, 0x4
