.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching InitHeap, 0xC

glabel InitHeap
    .incbin "expected/code.bin", 0x7A388, 0xC
endlabel InitHeap
    .incbin "expected/code.bin", 0x7A394, 0x4
