.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching _padChkVsync, 0x14

glabel _padChkVsync
    .incbin "expected/code.bin", 0x11CBDC, 0x14
endlabel _padChkVsync
