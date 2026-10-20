.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching CdControl, 0x13C

glabel CdControl
    .incbin "expected/code.bin", 0x11521C, 0x13C
endlabel CdControl
