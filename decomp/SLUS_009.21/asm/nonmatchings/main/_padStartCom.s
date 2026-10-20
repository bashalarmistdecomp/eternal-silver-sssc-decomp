.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching _padStartCom, 0xCC

glabel _padStartCom
    .incbin "expected/code.bin", 0x11CBF0, 0xCC
endlabel _padStartCom
