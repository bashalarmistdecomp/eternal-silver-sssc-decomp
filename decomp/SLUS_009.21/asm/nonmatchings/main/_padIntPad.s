.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching _padIntPad, 0x190

glabel _padIntPad
    .incbin "expected/code.bin", 0x11CA4C, 0x190
endlabel _padIntPad
