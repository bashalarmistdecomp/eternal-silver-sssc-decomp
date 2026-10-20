.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching CdReset, 0x6C

glabel CdReset
    .incbin "expected/code.bin", 0x1150AC, 0x6C
endlabel CdReset
