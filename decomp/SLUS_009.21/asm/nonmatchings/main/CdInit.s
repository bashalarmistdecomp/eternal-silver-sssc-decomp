.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching CdInit, 0x90

glabel CdInit
    .incbin "expected/code.bin", 0x114F10, 0x90
endlabel CdInit
