.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching CD_ready, 0x2C8

glabel CD_ready
    .incbin "expected/code.bin", 0x115E64, 0x2C8
endlabel CD_ready
