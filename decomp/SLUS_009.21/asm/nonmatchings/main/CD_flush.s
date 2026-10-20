.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching CD_flush, 0xD4

glabel CD_flush
    .incbin "expected/code.bin", 0x1165C0, 0xD4
endlabel CD_flush
