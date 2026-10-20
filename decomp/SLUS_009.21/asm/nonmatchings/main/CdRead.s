.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching CdRead, 0x19C

glabel CdRead
    .incbin "expected/code.bin", 0x117E60, 0x19C
endlabel CdRead
