.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching GsGetTimInfo, 0xE4

glabel GsGetTimInfo
    .incbin "expected/code.bin", 0x114DF0, 0xE4
endlabel GsGetTimInfo
    .incbin "expected/code.bin", 0x114ED4, 0xC
