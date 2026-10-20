.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching _padIsVsync, 0x68

glabel _padIsVsync
    .incbin "expected/code.bin", 0x11C9E4, 0x68
endlabel _padIsVsync
