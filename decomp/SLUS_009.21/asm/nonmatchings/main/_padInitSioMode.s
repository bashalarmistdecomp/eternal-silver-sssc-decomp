.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching _padInitSioMode, 0x334

glabel _padInitSioMode
    .incbin "expected/code.bin", 0x11CD00, 0x334
endlabel _padInitSioMode
