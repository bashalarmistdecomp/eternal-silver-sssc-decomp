.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching CdSearchFile, 0x2D8

glabel CdSearchFile
    .incbin "expected/code.bin", 0x116DF0, 0x2D8
endlabel CdSearchFile
