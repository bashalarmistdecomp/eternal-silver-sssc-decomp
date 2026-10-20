.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching CdControlB, 0x14C

glabel CdControlB
    .incbin "expected/code.bin", 0x11548C, 0x14C
endlabel CdControlB
