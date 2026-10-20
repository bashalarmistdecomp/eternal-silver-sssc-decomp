.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching CD_init, 0x1E0

glabel CD_init
    .incbin "expected/code.bin", 0x1167D0, 0x1E0
endlabel CD_init
