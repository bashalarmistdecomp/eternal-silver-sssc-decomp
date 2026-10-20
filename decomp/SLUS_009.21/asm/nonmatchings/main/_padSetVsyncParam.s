.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching _padSetVsyncParam, 0x2C

glabel _padSetVsyncParam
    .incbin "expected/code.bin", 0x11C9B8, 0x2C
endlabel _padSetVsyncParam
