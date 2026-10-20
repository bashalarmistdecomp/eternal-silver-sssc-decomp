.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching FntFlush, 0x31C

glabel FntFlush
    .incbin "expected/code.bin", 0x110618, 0x31C
endlabel FntFlush
