.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching _padSendAtLoadInfo, 0x84

glabel _padSendAtLoadInfo
    .incbin "expected/code.bin", 0x11D68C, 0x84
endlabel _padSendAtLoadInfo
