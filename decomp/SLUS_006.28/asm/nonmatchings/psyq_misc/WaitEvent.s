.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching WaitEvent, 0xC

glabel WaitEvent
    .incbin "expected/code.bin", 0x8F1C8, 0xC
endlabel WaitEvent
    .incbin "expected/code.bin", 0x8F1D4, 0x4
