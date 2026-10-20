.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching _padStopCom, 0x44

glabel _padStopCom
    .incbin "expected/code.bin", 0x11CCBC, 0x44
endlabel _padStopCom
