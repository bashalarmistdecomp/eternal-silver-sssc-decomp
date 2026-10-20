.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching CdReadBreak, 0x1A0

glabel CdReadBreak
    .incbin "expected/code.bin", 0x117CC0, 0x1A0
endlabel CdReadBreak
