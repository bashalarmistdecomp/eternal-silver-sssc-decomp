.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching memset, 0x30

glabel memset
    .incbin "expected/code.bin", 0x11FD20, 0x30
endlabel memset
