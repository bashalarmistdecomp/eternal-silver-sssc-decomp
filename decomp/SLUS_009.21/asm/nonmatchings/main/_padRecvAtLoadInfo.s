.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching _padRecvAtLoadInfo, 0x270

glabel _padRecvAtLoadInfo
    .incbin "expected/code.bin", 0x11D710, 0x270
endlabel _padRecvAtLoadInfo
