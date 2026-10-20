.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching VSync, 0x178

glabel VSync
    .incbin "expected/code.bin", 0x10F0C0, 0x178
endlabel VSync
