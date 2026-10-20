.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching strncmp, 0x80

glabel strncmp
    .incbin "expected/code.bin", 0x11FE40, 0x80
endlabel strncmp
