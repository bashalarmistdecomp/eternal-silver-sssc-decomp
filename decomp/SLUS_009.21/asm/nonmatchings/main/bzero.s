.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching bzero, 0x30

glabel bzero
    .incbin "expected/code.bin", 0x11FCB0, 0x30
endlabel bzero
