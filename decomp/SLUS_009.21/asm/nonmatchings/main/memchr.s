.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching memchr, 0x50

glabel memchr
    .incbin "expected/code.bin", 0x120610, 0x50
endlabel memchr
