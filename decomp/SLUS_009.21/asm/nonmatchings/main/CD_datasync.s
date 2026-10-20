.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching CD_datasync, 0x168

glabel CD_datasync
    .incbin "expected/code.bin", 0x1169B0, 0x168
endlabel CD_datasync
