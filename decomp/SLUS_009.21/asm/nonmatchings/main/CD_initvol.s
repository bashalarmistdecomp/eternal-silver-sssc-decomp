.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching CD_initvol, 0xF0

glabel CD_initvol
    .incbin "expected/code.bin", 0x116694, 0xF0
endlabel CD_initvol
